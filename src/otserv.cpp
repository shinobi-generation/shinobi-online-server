////////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////
#include "otpch.h"
#include "otsystem.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#ifndef WINDOWS
#include <unistd.h>
#endif
#include <boost/config.hpp>

#include "server.h"
#ifdef __LOGIN_SERVER__
#include "gameservers.h"
#endif
#include "networkmessage.h"

#include "game.h"
#include "chat.h"
#include "tools.h"
#include "rsa.h"
#include "textlogger.h"

#include "protocollogin.h"
#include "protocolgame.h"
#include "protocolold.h"
#include "status.h"
#ifdef __REMOTE_CONTROL__
#include "admin.h"
#endif

#include "configmanager.h"
#include "scriptmanager.h"
#include "databasemanager.h"

#include "iologindata.h"
#include "ioban.h"

#include "outfit.h"
#include "vocation.h"
#include "group.h"

#include "monsters.h"
#ifdef __OTSERV_ALLOCATOR__
#include "allocator.h"
#endif
#ifdef __EXCEPTION_TRACER__
#include "exception.h"
#endif

#include "resources.h"


#ifdef __NO_BOOST_EXCEPTIONS__
#include <exception>

void boost::throw_exception(std::exception const & e)
{
	std::cout << "Boost exception: " << e.what() << std::endl;
}
#endif

ConfigManager g_config;
Game g_game;
Monsters g_monsters;
Npcs g_npcs;

RSA g_RSA;
Chat g_chat;

IpList serverIps;
boost::mutex g_loaderLock;
boost::condition_variable g_loaderSignal;

boost::unique_lock<boost::mutex> g_loaderUniqueLock(g_loaderLock);
#ifdef __REMOTE_CONTROL__
extern Admin* g_admin;
#endif

bool argumentsHandler(StringVec args)
{
	StringVec tmp;
	for(StringVec::iterator it = args.begin(); it != args.end(); ++it)
	{
		if((*it) == "--help")
		{
			std::cout << "Usage:\n"
			"\n"
			"\t--config=$1\t\tAlternate configuration file path.\n"
			"\t--data-directory=$1\tAlternate data directory path.\n"
			"\t--ip=$1\t\t\tIP address of gameworld server.\n"
			"\t\t\t\tShould be equal to the global IP.\n"
			"\t--login-port=$1\tPort for login server to listen on.\n"
			"\t--game-port=$1\tPort for game server to listen on.\n"
			"\t--admin-port=$1\tPort for admin server to listen on.\n"
			"\t--status-port=$1\tPort for status server to listen on.\n";
#ifndef WINDOWS
			std::cout << "\t--runfile=$1\t\tSpecifies run file. Will contain the pid\n"
			"\t\t\t\tof the server process as long as it is running.\n";
#endif
			std::cout << "\t--output-log=$1\t\tAll standard output will be logged to\n"
			"\t\t\t\tthis file.\n"
			"\t--error-log=$1\t\tAll standard errors will be logged to\n"
			"\t\t\t\tthis file.\n";
			return false;
		}

		if((*it) == "--version")
		{
			std::cout << STATUS_SERVER_NAME << ", version " << STATUS_SERVER_VERSION << " (" << STATUS_SERVER_CODENAME << ")\n"
			"Compiled with " << BOOST_COMPILER << " at " << __DATE__ << ", " << __TIME__ << ".\n"
			"A server developed by Elf, slawkens, Talaturen, Lithium, KaczooH, Kiper, Kornholijo.\n"
			"Visit our forum for updates, support and resources: http://otland.net.\n";
			return false;
		}

		tmp = explodeString((*it), "=");
		if(tmp[0] == "--config")
			g_config.setString(ConfigManager::CONFIG_FILE, tmp[1]);
		else if(tmp[0] == "--data-directory")
			g_config.setString(ConfigManager::DATA_DIRECTORY, tmp[1]);
		else if(tmp[0] == "--ip")
			g_config.setString(ConfigManager::IP, tmp[1]);
		else if(tmp[0] == "--login-port")
			g_config.setNumber(ConfigManager::LOGIN_PORT, atoi(tmp[1].c_str()));
		else if(tmp[0] == "--game-port")
			g_config.setNumber(ConfigManager::GAME_PORT, atoi(tmp[1].c_str()));
		else if(tmp[0] == "--admin-port")
			g_config.setNumber(ConfigManager::ADMIN_PORT, atoi(tmp[1].c_str()));
		else if(tmp[0] == "--status-port")
			g_config.setNumber(ConfigManager::STATUS_PORT, atoi(tmp[1].c_str()));
#ifndef WINDOWS
		else if(tmp[0] == "--runfile")
			g_config.setString(ConfigManager::RUNFILE, tmp[1]);
#endif
		else if(tmp[0] == "--output-log")
			g_config.setString(ConfigManager::OUT_LOG, tmp[1]);
		else if(tmp[0] == "--error-log")
			g_config.setString(ConfigManager::ERROR_LOG, tmp[1]);
	}

	return true;
}

#ifdef WINDOWS
BOOL consoleHandler(DWORD signal) {

	switch (signal) {
		case CTRL_C_EVENT: {
			Dispatcher::getInstance().addTask(createTask(
				boost::bind(&Game::setGameState, &g_game, GAME_STATE_SHUTDOWN)));
			break;
		}
		case CTRL_SHUTDOWN_EVENT: {
			Dispatcher::getInstance().addTask(createTask(
				boost::bind(&Game::shutdown, &g_game)));
		}
	}
	return TRUE;
};
#else
void signalHandler(int32_t sig)
{
	uint32_t tmp = 0;
	switch(sig)
	{
		case SIGHUP:
			Dispatcher::getInstance().addTask(createTask(
				boost::bind(&Game::saveGameState, &g_game, false)));
			break;

		case SIGTRAP:
			g_game.cleanMap(tmp);
			break;

		case SIGCHLD:
			g_game.proceduralRefresh();
			break;

		case SIGUSR1:
			Dispatcher::getInstance().addTask(createTask(
				boost::bind(&Game::setGameState, &g_game, GAME_STATE_CLOSED)));
			break;

		case SIGUSR2:
			g_game.setGameState(GAME_STATE_NORMAL);
			break;

		case SIGCONT:
			g_game.reloadInfo(RELOAD_ALL);
			break;

		case SIGQUIT:
			Dispatcher::getInstance().addTask(createTask(
				boost::bind(&Game::setGameState, &g_game, GAME_STATE_SHUTDOWN)));
			break;

		case SIGTERM:
			Dispatcher::getInstance().addTask(createTask(
				boost::bind(&Game::shutdown, &g_game)));
			break;

		default:
			break;
	}
}

void runfileHandler(void)
{
	std::ofstream runfile(g_config.getString(ConfigManager::RUNFILE).c_str(), std::ios::trunc | std::ios::out);
	runfile.close();
}
#endif

void allocationHandler()
{
	puts("Allocation failed, server out of memory!\nDecrease size of your map or compile in a 64-bit mode.");
	char buffer[1024];
	fgets(buffer, 1024, stdin);
	exit(-1);
}

void startupErrorMessage(const std::string& error)
{
	if(error.length() > 0)
		std::cout << std::endl << "> ERROR: " << error << std::endl;

	getchar();
	exit(-1);
}

void otserv(
StringVec args,
ServiceManager* services);

int main(int argc, char *argv[])
{
	StringVec args = StringVec(argv, argv + argc);
	if(argc > 1 && !argumentsHandler(args))
		return 0;

	std::set_new_handler(allocationHandler);
	ServiceManager servicer;
	g_config.startup();

	#ifdef __OTSERV_ALLOCATOR_STATS__
	boost::thread(boost::bind(&allocatorStatsThread, (void*)NULL));
	// TODO: destruct this thread...
	#endif
	#ifdef __EXCEPTION_TRACER__
	ExceptionHandler mainExceptionHandler;
	mainExceptionHandler.InstallHandler();
	#endif
	#ifdef WINDOWS
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleHandler, TRUE);
	#else

	// ignore sigpipe...
	struct sigaction sigh;
	sigh.sa_handler = SIG_IGN;
	sigh.sa_flags = 0;
	sigemptyset(&sigh.sa_mask);
	sigaction(SIGPIPE, &sigh, NULL);

	// register signals
	signal(SIGHUP, signalHandler); //save
	signal(SIGTRAP, signalHandler); //clean
	signal(SIGCHLD, signalHandler); //refresh
	signal(SIGUSR1, signalHandler); //close server
	signal(SIGUSR2, signalHandler); //open server
	signal(SIGCONT, signalHandler); //reload all
	signal(SIGQUIT, signalHandler); //save & shutdown
	signal(SIGTERM, signalHandler); //shutdown
	#endif

	Dispatcher::getInstance().addTask(createTask(boost::bind(otserv,
	args,
	&servicer)));
	g_loaderSignal.wait(g_loaderUniqueLock);

	std::string outPath = g_config.getString(ConfigManager::OUT_LOG), errPath = g_config.getString(ConfigManager::ERROR_LOG);
	if(outPath.length() < 3)
		outPath = "";
	else if(outPath[0] != '/' && outPath[1] != ':')
	{
		outPath = getFilePath(FILE_TYPE_LOG, outPath);
		std::cout << "> Logging output to file: " << outPath << std::endl;
	}

	if(errPath.length() < 3)
		errPath = "";
	else if(errPath[0] != '/' && errPath[1] != ':')
	{
		errPath = getFilePath(FILE_TYPE_LOG, errPath);
		std::cout << "> Logging errors to file: " << errPath << std::endl;
	}

	if(outPath != "")
	{
		boost::shared_ptr<std::ofstream> outFile;
		outFile.reset(new std::ofstream(outPath.c_str(), (g_config.getBool(ConfigManager::TRUNCATE_LOGS) ?
			std::ios::trunc : std::ios::app) | std::ios::out));
		if(!outFile->is_open())
			startupErrorMessage("Could not open output log file for writing!");

		std::cout.rdbuf(outFile->rdbuf());
	}

	if(errPath != "")
	{
		boost::shared_ptr<std::ofstream> errFile;
		errFile.reset(new std::ofstream(errPath.c_str(), (g_config.getBool(ConfigManager::TRUNCATE_LOGS) ?
			std::ios::trunc : std::ios::app) | std::ios::out));
		if(!errFile->is_open())
			startupErrorMessage("Could not open error log file for writing!");

		std::cerr.rdbuf(errFile->rdbuf());
	}

	if(servicer.isRunning())
	{
		std::cout << ">> " << g_config.getString(ConfigManager::SERVER_NAME) << " server Online!" << std::endl << std::endl;
		servicer.run();
	}
	else
	{
		std::cout << ">> " << g_config.getString(ConfigManager::SERVER_NAME) << " server Offline! No services available..." << std::endl << std::endl;
	}

#ifdef __EXCEPTION_TRACER__
	mainExceptionHandler.RemoveHandler();
#endif
	return 0;
}

void otserv(
StringVec args,
ServiceManager* services)
{
	srand((uint32_t)OTSYS_TIME());
	#if defined(WINDOWS)
	SetConsoleTitle(STATUS_SERVER_NAME);
	#endif

	g_game.setGameState(GAME_STATE_STARTUP);
	#if !defined(WINDOWS) && !defined(__ROOT_PERMISSION__)
	if(!getuid() || !geteuid())
	{
		std::cout << "> WARNING: " << STATUS_SERVER_NAME << " has been executed as root user! It is recommended to execute as a normal user." << std::endl;
		std::cout << "Continue? (y/N)" << std::endl;

		char buffer = getchar();
		if(buffer == 10 || (buffer != 121 && buffer != 89))
			startupErrorMessage("Aborted.");
	}
	#endif

	std::cout << STATUS_SERVER_NAME << ", version " << STATUS_SERVER_VERSION << " (" << STATUS_SERVER_CODENAME << ")" << std::endl;
	std::cout << "Compiled with " << BOOST_COMPILER << " at " << __DATE__ << ", " << __TIME__ << "." << std::endl;
	std::cout << "A server developed by PHantom, based on TFS 0.3.6 by Elf, slawkens, Talaturen, KaczooH, Lithium, Kiper, Kornholijo." << std::endl;
	std::cout << "Visit the github repository for updates, support and resources: https://github.com/gilmarxd/shinobi-online-server." << std::endl << std::endl;

	std::stringstream ss;
	#ifdef __DEBUG__
	ss << " GLOBAL";
	#endif
	#ifdef __DEBUG_MOVESYS__
	ss << " MOVESYS";
	#endif
	#ifdef __DEBUG_CHAT__
	ss << " CHAT";
	#endif
	#ifdef __DEBUG_EXCEPTION_REPORT__
	ss << " EXCEPTION-REPORT";
	#endif
	#ifdef __DEBUG_HOUSES__
	ss << " HOUSES";
	#endif
	#ifdef __DEBUG_LUASCRIPTS__
	ss << " LUA-SCRIPTS";
	#endif
	#ifdef __DEBUG_MAILBOX__
	ss << " MAILBOX";
	#endif
	#ifdef __DEBUG_NET__
	ss << " NET";
	#endif
	#ifdef __DEBUG_NET_DETAIL__
	ss << " NET-DETAIL";
	#endif
	#ifdef __DEBUG_RAID__
	ss << " RAIDS";
	#endif
	#ifdef __DEBUG_SCHEDULER__
	ss << " SCHEDULER";
	#endif
	#ifdef __DEBUG_SPAWN__
	ss << " SPAWNS";
	#endif
	#ifdef __SQL_QUERY_DEBUG__
	ss << " SQL-QUERIES";
	#endif

	std::string debug = ss.str();
	if(!debug.empty())
	{
		std::cout << ">> Debugging:";
		std::cout << debug << "." << std::endl;
	}

	std::cout << ">> Loading config (" << g_config.getString(ConfigManager::CONFIG_FILE) << ")" << std::endl;
	if(!g_config.load())
		startupErrorMessage("Unable to load " + g_config.getString(ConfigManager::CONFIG_FILE) + "!");

	Logger::getInstance()->open();
	IntegerVec cores = vectorAtoi(explodeString(g_config.getString(ConfigManager::CORES_USED), ","));
	if(cores[0] != -1)
	{
	#ifdef WINDOWS
		int32_t mask = 0;
		for(IntegerVec::iterator it = cores.begin(); it != cores.end(); ++it)
			mask += 1 << (*it);

		SetProcessAffinityMask(GetCurrentProcess(), mask);
	}

	std::stringstream mutexName;
	mutexName << "forgottenserver_" << g_config.getNumber(ConfigManager::WORLD_ID);

	CreateMutex(NULL, FALSE, mutexName.str().c_str());
	if(GetLastError() == ERROR_ALREADY_EXISTS)
		startupErrorMessage("Another instance of The Forgotten Server is already running with the same worldId.\nIf you want to run multiple servers, please change the worldId in configuration file.");

	std::string defaultPriority = asLowerCaseString(g_config.getString(ConfigManager::DEFAULT_PRIORITY));
	if(defaultPriority == "realtime")
		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	else if(defaultPriority == "high")
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	else if(defaultPriority == "higher")
		SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	#else
		cpu_set_t mask;
		CPU_ZERO(&mask);
		for(IntegerVec::iterator it = cores.begin(); it != cores.end(); ++it)
			CPU_SET((*it), &mask);

		sched_setaffinity(getpid(), (int32_t)sizeof(mask), &mask);
	}

	std::string runPath = g_config.getString(ConfigManager::RUNFILE);
	if(runPath != "" && runPath.length() > 2)
	{
		std::ofstream runFile(runPath.c_str(), std::ios::trunc | std::ios::out);
		runFile << getpid();
		runFile.close();
		atexit(runfileHandler);
	}

	if(!nice(g_config.getNumber(ConfigManager::NICE_LEVEL))) {}
	#endif
	std::string encryptionType = asLowerCaseString(g_config.getString(ConfigManager::ENCRYPTION_TYPE));
	if(encryptionType == "md5")
	{
		g_config.setNumber(ConfigManager::ENCRYPTION, ENCRYPTION_MD5);
		std::cout << "> Using MD5 encryption" << std::endl;
	}
	else if(encryptionType == "sha1")
	{
		g_config.setNumber(ConfigManager::ENCRYPTION, ENCRYPTION_SHA1);
		std::cout << "> Using SHA1 encryption" << std::endl;
	}
	else
	{
		g_config.setNumber(ConfigManager::ENCRYPTION, ENCRYPTION_PLAIN);
		std::cout << "> Using plaintext encryption" << std::endl;
	}
	
	std::cout << ">> Checking software version... ";
	if(xmlDocPtr doc = xmlReadFile(VERSION_CHECK, "UTF-8", 0))
	{
		xmlNodePtr p, root = xmlDocGetRootElement(doc);
		if(!xmlStrcmp(root->name, (const xmlChar*)"versions"))
		{
			p = root->children->next;
			if(!xmlStrcmp(p->name, (const xmlChar*)"entry"))
			{
				std::string version;
				int32_t patch, build, timestamp;

				bool tmp = false;
				if(readXMLString(p, "version", version) && version != STATUS_SERVER_VERSION)
					tmp = true;

				if(readXMLInteger(p, "patch", patch) && patch > VERSION_PATCH)
					tmp = true;

				if(readXMLInteger(p, "build", build) && build > VERSION_BUILD)
					tmp = true;

				if(readXMLInteger(p, "timestamp", timestamp) && timestamp > VERSION_TIMESTAMP)
					tmp = true;

				if(tmp)
				{
					std::cout << "outdated, please consider updating!" << std::endl;
					std::cout << "> Current version information - version: " << STATUS_SERVER_VERSION << ", patch: " << VERSION_PATCH;
					std::cout << ", build: " << VERSION_BUILD << ", timestamp: " << VERSION_TIMESTAMP << "." << std::endl;
					std::cout << "> Latest version information - version: " << version << ", patch: " << patch;
					std::cout << ", build: " << build << ", timestamp: " << timestamp << "." << std::endl;
					if(g_config.getBool(ConfigManager::CONFIM_OUTDATED_VERSION) && version.find("_SVN") == std::string::npos)
					{
						std::cout << "Continue? (y/N)" << std::endl;
	
						char buffer = getchar();
						if(buffer == 10 || (buffer != 121 && buffer != 89))
							startupErrorMessage("Aborted.");
					}
				}
				else
					std::cout << "up to date!" << std::endl;
			}
			else
				std::cout << "failed checking - malformed entry." << std::endl;
		}
		else
			std::cout << "failed checking - malformed file." << std::endl;

		xmlFreeDoc(doc);
	}
	else
		std::cout << "failed - could not parse remote file (are you connected to the internet?)" << std::endl;

	/*
	std::cout << ">> Fetching blacklist" << std::endl;
	if(!g_game.fetchBlacklist())
	{

		std::cout << "Unable to fetch blacklist! Continue? (y/N)" << std::endl;
		char buffer = getchar();
		if(buffer == 10 || (buffer != 121 && buffer != 89))
			startupErrorMessage("Unable to fetch blacklist!");
	}*/

	std::cout << ">> Loading RSA key" << std::endl;

	const char* p("14299623962416399520070177382898895550795403345466153217470516082934737582776038882967213386204600674145392845853859217990626450972452084065728686565928113");
	const char* q("7630979195970404721891201847792002125535401292779123937207447574596692788513647179235335529307251350570728407373705564708871762033017096809910315212884101");
	const char* d("46730330223584118622160180015036832148732986808519344675210555262940258739805766860224610646919605860206328024326703361630109888417839241959507572247284807035235569619173792292786907845791904955103601652822519121908367187885509270025388641700821735345222087940578381210879116823013776808975766851829020659073");
	g_RSA.setKey(p, q, d);

	std::cout << ">> Starting SQL connection" << std::endl;
	Database* db = Database::getInstance();
	if(db && db->isConnected())
	{
		std::cout << ">> Running Database Manager" << std::endl;
		if(!DatabaseManager::getInstance()->isDatabaseSetup())
			startupErrorMessage("The database you specified in config.lua is empty, please import schemas/<dbengine>.sql to the database (if you are using MySQL, please read doc/MYSQL_HELP for more information).");
		else
		{
			uint32_t version = 0;
			do
			{
				version = DatabaseManager::getInstance()->updateDatabase();
				if(version == 0)
					break;

				std::cout << "> Database has been updated to version: " << version << "." << std::endl;
			}
			while(version < VERSION_DATABASE);
		}

		DatabaseManager::getInstance()->checkTriggers();
		DatabaseManager::getInstance()->checkEncryption();
		if(g_config.getBool(ConfigManager::OPTIMIZE_DB_AT_STARTUP) && !DatabaseManager::getInstance()->optimizeTables())
			std::cout << "> No tables were optimized." << std::endl;
	}
	else
		startupErrorMessage("Couldn't estabilish connection to SQL database!");

	std::cout << ">> Loading items" << std::endl;
	if(Item::items.loadFromOtb(getFilePath(FILE_TYPE_OTHER, "items/items.otb")))
		startupErrorMessage("Unable to load items (OTB)!");

	if(!Item::items.loadFromXml())
	{

		std::cout << "Unable to load items (XML)! Continue? (y/N)" << std::endl;
		char buffer = getchar();
		if(buffer == 10 || (buffer != 121 && buffer != 89))
		startupErrorMessage("Unable to load items (XML)!");
	}

	std::cout << ">> Loading groups" << std::endl;
	if(!Groups::getInstance()->loadFromXml())
		startupErrorMessage("Unable to load groups!");

	std::cout << ">> Loading vocations" << std::endl;
	if(!Vocations::getInstance()->loadFromXml())
		startupErrorMessage("Unable to load vocations!");

	std::cout << ">> Loading script systems" << std::endl;
	if(!ScriptingManager::getInstance()->load())
		startupErrorMessage("");

	std::cout << ">> Loading chat channels" << std::endl;
	if(!g_chat.loadFromXml())
		startupErrorMessage("Unable to load chat channels!");

	std::cout << ">> Loading outfits" << std::endl;
	if(!Outfits::getInstance()->loadFromXml())
		startupErrorMessage("Unable to load outfits!");

	std::cout << ">> Loading experience stages" << std::endl;
	if(!g_game.loadExperienceStages())
		startupErrorMessage("Unable to load experience stages!");

	std::cout << ">> Loading monsters" << std::endl;
	if(!g_monsters.loadFromXml())
	{
		std::cout << "Unable to load monsters! Continue? (y/N)" << std::endl;
		char buffer = getchar();
		if(buffer == 10 || (buffer != 121 && buffer != 89))
			startupErrorMessage("Unable to load monsters!");
	}

	std::cout << ">> Loading mods..." << std::endl;
	if(!ScriptingManager::getInstance()->loadMods())
		startupErrorMessage("Unable to load mods!");

	std::cout << ">> Loading map and spawns..." << std::endl;
	if(!g_game.loadMap(g_config.getString(ConfigManager::MAP_NAME)))
		startupErrorMessage("");

	#ifdef __LOGIN_SERVER__
	std::cout << ">> Loading game servers" << std::endl;
	if(!GameServers::getInstance()->loadFromXml(true))
		startupErrorMessage("Unable to load game servers!");
	#endif

	#ifdef __REMOTE_CONTROL__
	std::cout << ">> Loading administration protocol" << std::endl;

	g_admin = new Admin();
	if(!g_admin->loadFromXml())
		startupErrorMessage("Unable to load administration protocol!");

	services->add<ProtocolAdmin>(g_config.getNumber(ConfigManager::ADMIN_PORT));
	#endif

	std::cout << ">> Checking world type... ";
	std::string worldType = asLowerCaseString(g_config.getString(ConfigManager::WORLD_TYPE));
	if(worldType == "pvp" || worldType == "2" || worldType == "normal")
	{
		g_game.setWorldType(WORLD_TYPE_PVP);
		std::cout << "PvP" << std::endl;
	}
	else if(worldType == "no-pvp" || worldType == "nopvp" || worldType == "non-pvp" || worldType == "nonpvp" || worldType == "1" || worldType == "safe")
	{
		g_game.setWorldType(WORLD_TYPE_NO_PVP);
		std::cout << "NoN-PvP" << std::endl;
	}
	else if(worldType == "pvp-enforced" || worldType == "pvpenforced" || worldType == "pvp-enfo" || worldType == "pvpenfo" || worldType == "pvpe" || worldType == "enforced" || worldType == "enfo" || worldType == "3" || worldType == "war")
	{
		g_game.setWorldType(WORLD_TYPE_PVP_ENFORCED);
		std::cout << "PvP-Enforced" << std::endl;
	}
	else
	{
		std::cout << std::endl;
		startupErrorMessage("Unknown world type: " + g_config.getString(ConfigManager::WORLD_TYPE));
	}

	std::cout << ">> Initializing game state modules and registering services..." << std::endl;
	g_game.setGameState(GAME_STATE_INIT);

	std::string ip = g_config.getString(ConfigManager::IP);
	std::cout << "> Global address: " << ip << std::endl;
	serverIps.push_back(std::make_pair(LOCALHOST, 0xFFFFFFFF));

	char hostName[128];
	hostent* host = NULL;
	if(!gethostname(hostName, 128) && (host = gethostbyname(hostName)))
	{
		uint8_t** address = (uint8_t**)host->h_addr_list;
		while(address[0] != NULL)
		{
			serverIps.push_back(std::make_pair(*(uint32_t*)(*address), 0x0000FFFF));
			address++;
		}
	}

	uint32_t resolvedIp = inet_addr(ip.c_str());
	if(resolvedIp == INADDR_NONE)
	{
		if((host = gethostbyname(ip.c_str())))
			resolvedIp = *(uint32_t*)host->h_addr;
		else
			startupErrorMessage("Cannot resolve " + ip + "!");
	}

	serverIps.push_back(std::make_pair(resolvedIp, 0));
	Status::getInstance()->setMapName(g_config.getString(ConfigManager::MAP_NAME));

	services->add<ProtocolStatus>(g_config.getNumber(ConfigManager::STATUS_PORT));
	if(
#ifdef __LOGIN_SERVER__
	true
#else
	!g_config.getBool(ConfigManager::LOGIN_ONLY_LOGINSERVER)
#endif
	)
	{
		services->add<ProtocolLogin>(g_config.getNumber(ConfigManager::LOGIN_PORT));
		services->add<ProtocolOldLogin>(g_config.getNumber(ConfigManager::LOGIN_PORT));
	}

	services->add<ProtocolGame>(g_config.getNumber(ConfigManager::GAME_PORT));
	services->add<ProtocolOldGame>(g_config.getNumber(ConfigManager::LOGIN_PORT));
	std::cout << "> Local ports: ";

	std::list<uint16_t> ports = services->getPorts();
	for(std::list<uint16_t>::iterator it = ports.begin(); it != ports.end(); ++it)
		std::cout << (*it) << "\t";

	std::cout << std::endl << ">> All modules were loaded, server is starting up..." << std::endl;
	g_game.setGameState(GAME_STATE_NORMAL);

	g_game.start(services);
	g_loaderSignal.notify_all();
}
