using ReactiveUI;
using System.Collections;
using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Net.Http.Json;
using System.Net.Http.Headers;
using System.Text;
using System.Text.Json.Nodes;
using System.Text.Json;
using System.Linq;
using System.Threading.Tasks;
using Tmds.DBus.Protocol;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Text.Json.Serialization;

namespace OfficeConfigurator.ViewModels
{
    public class newDevice
    {
        public int measStation { get; set; }
        public string[] buttStations { get; set; }
    }

    public class newGame
    {
        public string name { get; set; }
        public int members { get; set; }
    }

    public class minMax
    {
        public int min { get; set; }
        public int max { get; set; }
    }

    public class limits
    {
        public minMax temperatures { get; set; }
        public minMax humidity { get; set; }
    }

    public class MainWindowViewModel : ViewModelBase
    {
        public string StationId { set; get; } = "";
        public string DeskIds { set; get; } = "";
        public string ActivityName { set; get; } = "";
        public int ActivityPlayerCount { set; get; }

        public int TempMin { set; get; }
        public int TempMax { set; get; }

        public int HumMin { set; get; }
        public int HumMax { set; get; }

        HttpClient client = new HttpClient()
        {
            BaseAddress = new Uri("http://192.168.241.195:3000")
        };


        public string _debugString = "";
        public string DebugString
        {
            get
            {
                return _debugString;
            }
            set
            {
                this.RaiseAndSetIfChanged(ref _debugString, value);
            }
        }

        public MainWindowViewModel() : base()
        {
            client.DefaultRequestHeaders.Accept.Add(
                new MediaTypeWithQualityHeaderValue("application/json"));

            string thr = get("thresholds/", "");
            limits myDetails = JsonSerializer.Deserialize<limits>(thr);
            TempMin = myDetails.temperatures.min;
            TempMax = myDetails.temperatures.max;
            HumMin = myDetails.humidity.min;
            HumMax = myDetails.humidity.max;
        }

        public void ClickHandler()
        {
            string[] subs = DeskIds.Split(',');

            for (int i = 0; i < subs.Length; ++i)
            {
                subs[i] = String.Concat(subs[i].Where(c => !Char.IsWhiteSpace(c)));
                subs[i] = new string('0', 5 - subs[i].Length) + subs[i];
            }

            string urlParameters = "";

            try
            {
                var ni = new newDevice()
                {
                    measStation = Int32.Parse(StationId),
                    buttStations = subs
                };

                urlParameters = JsonSerializer.Serialize<newDevice>(ni);
            } catch (System.FormatException)
            {
                DebugString = "Measuring station ID must be int!";
                return;
            }

            post(urlParameters, "deviceMap/", "New station written succesfully!");
        }

        public void ClickGameHandler()
        {
            var ni = new newGame()
            {
                name = this.ActivityName,
                members = this.ActivityPlayerCount
                
            };

            string urlParameters = JsonSerializer.Serialize<newGame>(ni);

            post(urlParameters, "game/", "New activity added succesfully!");
        }

        public void ClickLimitsHandler()
        {
            var ni = new limits()
            {
                temperatures = new minMax()
                {
                    min = TempMin,
                    max = TempMax
                },
                humidity = new minMax()
                {
                    min = HumMin,
                    max = HumMax
                }

            };

            string urlParameters = JsonSerializer.Serialize<limits>(ni);

            post(urlParameters, "thresholds/", "Thresholds redefined succesfully!");
        }

        private void post(string urlParameters, string post_name, string comm)
        {
            HttpContent content = new StringContent(urlParameters, Encoding.UTF8, "application/json");

            var response = client.PostAsync(post_name, content);
            try
            {
                while (!response.IsCompleted)
                {
                }
                if (response.Result.IsSuccessStatusCode)
                {
                    DebugString = comm;
                }
                else
                {
                    DebugString = $"{(int)response.Result.StatusCode} ({response.Result.ReasonPhrase})";
                }
            }
            catch (System.AggregateException)
            {
                DebugString = "Server is not available!";
            }
        }

        private string get(string post_name, string comm)
        {
            //HttpContent content = new StringContent(urlParameters, Encoding.UTF8, "application/json");

            var response = client.GetAsync(post_name);
            try
            {
                while (!response.IsCompleted)
                {
                }
                if (response.Result.IsSuccessStatusCode)
                {
                    string? dataObjects = response.Result.Content.ReadAsStringAsync().Result;
                    DebugString = comm;
                    return dataObjects;
                }
                else
                {
                    DebugString = $"{(int)response.Result.StatusCode} ({response.Result.ReasonPhrase})";
                }
            }
            catch (System.AggregateException)
            {
                DebugString = "Server is not available!";
            }

            throw new Exception();
        }

        ~MainWindowViewModel()
        {
            client.Dispose();
        }
    }
}