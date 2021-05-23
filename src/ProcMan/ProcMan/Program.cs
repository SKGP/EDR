using System;
using System.Management;
using MySql.Data.MySqlClient;
using System.Diagnostics;
using System.Net.Http;
using RestSharp;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Linq;

/*
 * nuggies to download:
 * System.Data.MySqlClient
 * RestSharp
*/

namespace ProcMan
{
    class Program
    {
        //------mysql connectors------
        static MySqlCommand cmd_open;
        static MySqlCommand cmd_close;
        static MySqlConnection conn;
        static MySqlConnection conn2;

        //------VirusTotal------
        static string api_key = "<YOUR_API_KEY>";

        public static void Main()
        {
            // Console.WriteLine(calculateSha1(@"C:\\Windows\\system32\\winlogon.exe"));


            Process[] processCollection = Process.GetProcesses();

            string mysqlServer = @"server=localhost;userid=USER;password=PASS;database=ph0enix;";
            conn = new MySqlConnection(mysqlServer); //create connection
            conn2 = new MySqlConnection(mysqlServer); //create connection
            cmd_open = new MySqlCommand(); //create command controller
            cmd_close = new MySqlCommand(); //create command controller
            cmd_open.Connection = conn; //connect the controller to the db
            cmd_close.Connection = conn2; //connect the controller to the db


            foreach (Process p in processCollection)
            {
                try //process like SYSTEM and Idle has no path...
                {
                    Console.WriteLine(p.ProcessName + "->" + p.Id + "->" + p.MainModule.FileName);
                    update_process(p.ProcessName, p.Id, p.MainModule.FileName, calculateSha1(p.MainModule.FileName), "host-name");
                }
                catch
                {
                }


            }

            try
            {
                ManagementEventWatcher startWatch = new ManagementEventWatcher(
                    new WqlEventQuery("SELECT * FROM Win32_ProcessStartTrace"));
                startWatch.EventArrived += new EventArrivedEventHandler(startWatch_EventArrived);
                startWatch.Start();
                ManagementEventWatcher stopWatch = new ManagementEventWatcher(
                    new WqlEventQuery("SELECT * FROM Win32_ProcessStopTrace"));
                stopWatch.EventArrived += new EventArrivedEventHandler(stopWatch_EventArrived);
                stopWatch.Start();
                Console.WriteLine("Press any key to exit");
                while (!Console.KeyAvailable) System.Threading.Thread.Sleep(50);
                startWatch.Stop();
                stopWatch.Stop();
            }
            catch
            { //possible wrong path, I dunno I dont wanna take risk of this process failling :(
            }
        }

        static void stopWatch_EventArrived(object sender, EventArrivedEventArgs e)
        {
            
            conn2.Open();
            Console.WriteLine("Process stopped: {0}", e.NewEvent.Properties["ProcessName"].Value);
            Console.WriteLine("Process stopped: {0}", e.NewEvent.Properties["ProcessId"].Value);
            //Console.WriteLine("path: {0}", e.NewEvent.Properties["ExecutablePath"].Value);
            //cmd_close.CommandText = $"DELETE FROM processes WHERE hostname = 'hosttest' AND pid = {e.NewEvent.Properties["ProcessId"].Value}";
            cmd_close.CommandText = $"UPDATE processes SET active = 0 WHERE hostname = 'host-name' AND pid = {e.NewEvent.Properties["ProcessId"].Value}";
            cmd_close.ExecuteNonQuery();
            conn2.Close();
        }

        static void startWatch_EventArrived(object sender, EventArrivedEventArgs e)
        {

            const string wmiQueryString = "SELECT ProcessId, ExecutablePath, CommandLine FROM Win32_Process";
            using (var searcher = new ManagementObjectSearcher(wmiQueryString))
            using (var results = searcher.Get())
            {
                var query = from p in Process.GetProcesses()
                            join mo in results.Cast<ManagementObject>()
                                on (int)(uint)e.NewEvent.Properties["ProcessID"].Value equals (int)(uint)mo["ProcessId"]
                            select new
                            {
                                Process = p,
                                Path = (string)mo["ExecutablePath"],
                                CommandLine = (string)mo["CommandLine"],
                            };
                foreach (var item in query)
                {
                    conn.Open();
                    Console.WriteLine("Process started: {0}", e.NewEvent.Properties["ProcessName"].Value);
                    Console.WriteLine("Process started: {0}", e.NewEvent.Properties["ProcessId"].Value);
                    cmd_open.CommandText = $"INSERT INTO processes(hostname, pid, proc_name, path, sha1, active, rate) VALUES('host-name', '{e.NewEvent.Properties["ProcessId"].Value}', '{e.NewEvent.Properties["ProcessName"].Value}', '{item.Path}', '{calculateSha1(item.Path)}', 1, 0)";
                    //rate field will be changed in the agent
                    cmd_open.ExecuteNonQuery();
                    conn.Close();
                    break;
                }

                
            }
        }

        static void update_process(string processname, int pid, string path, string sha1, string hostname)
        {
            conn.Open();
            cmd_open.CommandText = $"INSERT INTO processes(hostname, pid, proc_name, path, sha1, active, rate) VALUES('{hostname}', {pid}, '{processname}', '{path}', '{sha1}', 1, 0)";
            cmd_open.ExecuteNonQuery();
            conn.Close();
        }


        static string calculateSha1(string path)
        {
            try
            {
                using (FileStream stream = File.OpenRead(path))
                {
                    SHA256Managed sha = new SHA256Managed();
                    byte[] hash = sha.ComputeHash(stream);
                    return BitConverter.ToString(hash).Replace("-", String.Empty);
                }
            }
            catch{ //possible wrong path 
                return "";
            }
            

        }


        //check specific file from 
        static void virusTotal_fileupload(string path)
        {
            var client = new RestClient("https://www.virustotal.com/vtapi/v2/file/scan");
            var request = new RestRequest(Method.POST);
            request.AddHeader("content-type", "application/x-www-form-urlencoded");
            request.AddHeader("cache-control", "no-cache");
            request.AddHeader("header1", "headerval");
            request.AddParameter("application/x-www-form-urlencoded", $"apikey={api_key}&file={path}", ParameterType.RequestBody);
            //request.AddParameter("application/x-www-form-urlencoded", $"file={path}", ParameterType.RequestBody);
            IRestResponse response = client.Execute(request);

            Console.WriteLine(response.Content);
        }

        static void virusTotal_hashcheck(string hash)
        {

        }
    }
}
