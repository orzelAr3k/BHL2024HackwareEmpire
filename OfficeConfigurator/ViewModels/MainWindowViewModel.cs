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

namespace OfficeConfigurator.ViewModels
{
    public class newDevice
    {
        public int measStation { get; set; }
        public string[] buttStations { get; set; }
    }

    public class MainWindowViewModel : ViewModelBase
    {
        public string StationId { set; get; } = "";
        public string DeskIds { set; get; } = "";
        public string ActivityName { set; get; } = "";
        public string ActivityPlayerCount { set; get; } = "";

        //public string _fullStationStr;
        //public string FullStationStr
        //{
        //    get
        //    {
        //        return _fullStationStr;
        //    }
        //    set
        //    {
        //        this.RaiseAndSetIfChanged(ref _fullStationStr, value);
        //    }
        //}

        public void ClickHandler()
        {

            string URL = "http://192.168.241.195:3000/deviceMap";

            var client = new HttpClient();
            client.BaseAddress = new Uri(URL);
            client.DefaultRequestHeaders.Accept.Add(
                new MediaTypeWithQualityHeaderValue("application/json"));

            string[] subs = DeskIds.Split(',');

            for (int i = 0; i < subs.Length; ++i)
            {
                subs[i] = String.Concat(subs[i].Where(c => !Char.IsWhiteSpace(c)));
                subs[i] = new string('0', 5 - subs[i].Length) + subs[i];
            }

            var ni = new newDevice()
            {
                measStation = Int32.Parse(StationId),
                buttStations = subs
            };

            string urlParameters = JsonSerializer.Serialize<newDevice>(ni);

            HttpContent content = new StringContent(urlParameters, Encoding.UTF8, "application/json");

            var response = client.PostAsync("deviceMap/", content);
            try {
                while (!response.IsCompleted)
                {
                }
                if (response.Result.IsSuccessStatusCode)
                {
                    //FullStationStr += "Positive response from the server";
                }
                else
                {
                    //FullStationStr += $"{(int)response.Result.StatusCode} ({response.Result.ReasonPhrase}) {response.Result.Content.ReadAsStringAsync().Result}";
                }
            }
            catch (System.AggregateException)
            {
                // TODO - popup
            }

            client.Dispose();
        }

        public void ClickGameHandler()
        {

        }
    }
}