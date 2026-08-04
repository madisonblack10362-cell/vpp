class ZenModLogger
{
	const static string LOG_FOLDER = "$profile:\\Zenarchist\\Logs\\";
	const static string LOG_FILE = "ZenMod";

	// Log timer
	private static ref Timer m_PvpLogTimer;
	private static ref array<string> PVP_LOG;

	void ZenModLogger()
	{
		m_PvpLogTimer = new Timer();
		m_PvpLogTimer.Run(30, this, "LogDelayedPVP", NULL, true);
		PVP_LOG = new array<string>;
	}

	static void Log(string txt, string type = "general")
	{
		string file_path = LOG_FOLDER + LOG_FILE;

		switch (type)
		{
			case "pvp":
				file_path = file_path + "_pvp.log";
				break;
			case "basebuild":
				file_path = file_path + "_basebuild.log";
				break;
			case "baseraid":
				file_path = file_path + "_baseraid.log";
				break;
			case "login":
				file_path = file_path + "_logins.log";
				break;
			case "vehicle":
				file_path = file_path + "_vehicle.log";
				break;
			case "mission":
				file_path = file_path + "_mission.log";
				break;
			case "notes":
				file_path = file_path + "_notes.log";
				break;
			case "quests":
				file_path = file_path + "_quests.log";
				break;
			case "trader":
				file_path = file_path + "_trader.log";
				break;
			default:
				file_path = file_path + "_general.log";
				break;
		}

		if (!FileExist(LOG_FOLDER))
		{ 
			// If log folder doesn't exist, create it.
			MakeDirectory(LOG_FOLDER);
		}

		if (file_path == "gunfight")
		{
			PVP_LOG.Insert(txt);
			return;
		}

		FileHandle logFile = OpenFile(file_path, FileMode.APPEND);
		if (logFile != 0)
		{
			FPrintln(logFile, GetDate() + " [ZenMod] " + txt);
			CloseFile(logFile);
		}
	}

	static void LogDelayedPVP()
	{
		string file_path = LOG_FOLDER + LOG_FILE + "_pvp.log";
		FileHandle logFile = OpenFile(file_path, FileMode.APPEND);
		if (logFile == 0)
			return;

		foreach(string s : PVP_LOG)
		{
			if (logFile != 0)
			{
				FPrintln(logFile, GetDate() + " [ZenMod] " + s);
			}
		}

		CloseFile(logFile);
		PVP_LOG.Clear();
	}

	static private string GetDate(bool fileFriendly = false)
	{
		int year, month, day, hour, minute, second;

		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);

		string date = day.ToStringLen(2) + "." + month.ToStringLen(2) + "." + year.ToStringLen(4) + " " + hour.ToStringLen(2) + ":" + minute.ToStringLen(2) + ":" + second.ToStringLen(2);
		if (fileFriendly)
		{
			date.Replace(" ", "_");
			date.Replace(".", "-");
			date.Replace(":", "-");
		}

		return date;
	}
}