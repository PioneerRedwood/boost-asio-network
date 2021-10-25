using System;
using System.Collections.Generic;
using System.Net.Http;
using UnityEngine;

public static class LoginClient
{
    public static bool TryLogin(string id, string pwd, ref Dictionary<string, string> dict)
    {
        try
        {
            HttpClient client = new HttpClient();
            string uri = "http://localhost:8081/signin/";
            client.BaseAddress = new Uri(uri);

            HttpResponseMessage response = client.GetAsync(uri + $"{id}/{pwd}").Result;
            if (response.IsSuccessStatusCode)
            {
                string result = response.Content.ReadAsStringAsync().Result;
                Debug.Log(result);
                result = result.Replace('{', ' ').Replace('}', ' ').Replace('\"', ' ').Replace('"', ' ').Trim();

                string[] splited1 = result.Split(':');
                dict.Add(splited1[0].Trim(), splited1[1].Trim());

                return true;
            }
            else
            {
                Debug.Log($"{(int)response.StatusCode} {response.ReasonPhrase}");
                return false;
            }
        }
        catch (Exception e)
        {
            Debug.Log(e);
            return false;
        }
    }
}