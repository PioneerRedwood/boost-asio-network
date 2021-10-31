using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using System.Collections.Concurrent;
using System.Text;

public class NetworkManager : MonoBehaviour
{
    [SerializeField]
    private Dictionary<string, string> userInfo = new Dictionary<string, string>();

    RedNetwork.LobbyClient lobbyClient;

    ConcurrentQueue<string> queue = new ConcurrentQueue<string>();

	public string GetUserInfo(string key)
	{
        return userInfo[key];
	}

	private void Awake()
	{
        SceneManager.sceneLoaded += OnSceneLoaded;

        NetworkManager[] those = FindObjectsOfType<NetworkManager>();

        if(those.Length > 1)
		{
            Destroy(this.gameObject);
		}

        DontDestroyOnLoad(gameObject);
	}

	void Start()
    {
		InitLobbyClient();
	}

    void Update()
    {
        
    }

    private void InitLobbyClient()
	{
        lobbyClient = new RedNetwork.LobbyClient(ref queue);
        lobbyClient.Connect("127.0.0.1", 9000);

        if (!IsInvoking(nameof(CheckMessageFromServer)))
        {
            InvokeRepeating(nameof(CheckMessageFromServer), 3.0f, 0.5f);
        }

        InvokeRepeating(nameof(PingToServer), 1.0f, 1.0f);
    }

    RedNetwork.PingPacket pingPacket = new RedNetwork.PingPacket(0, "ping");
    private void PingToServer()
	{
        //lobbyClient.Send();
        lobbyClient.Send(Encoding.ASCII.GetString(pingPacket.serialize()));
	}

    public void OnLoginButtonClicked()
	{
        string id = GameObject.Find("IDText").GetComponent<Text>().text;
        string pwd = GameObject.Find("PWDText").GetComponent<Text>().text;

        Debug.Log($"ID{id} PWD{pwd}");
        if (LoginClient.TryLogin(id, pwd, ref userInfo))
        {
            SceneManager.LoadScene("LobbyScene");
        }
    }

    public void OnSceneLoaded(Scene scene, LoadSceneMode mode)
	{
        if (scene.name.Equals("LobbyScene"))
        {
            try
            {
                // here, we need to activate[Instantiate/Create/Spawn ... ] our network client instance
                GameObject.Find("IDText").GetComponent<Text>().text = userInfo["id"];

                lobbyClient = new RedNetwork.LobbyClient(ref queue);
                lobbyClient.Connect("127.0.0.1", 9000);

                InvokeRepeating(nameof(lobbyClient.PingToServer), 3.0f, 1.0f);
                InvokeRepeating(nameof(CheckMessageFromServer), 3.0f, 0.5f);
            }
            catch (Exception e)
            {
                Debug.Log(e);
            }
        }
    }

    void CheckMessageFromServer()
	{
		if (queue.TryDequeue(out string result))
		{
			Debug.Log(result);
			// 정보 가져오기
			if (result.Contains("lobby"))
            {
                if (LobbyUIManager.Instance.CreateLobbies(result))
				{
                    LobbyUIManager.Instance.InitUI();
				}
			}
		}
	}

    public void OnRefreshButtonClicked()
	{
        if (lobbyClient.Connected())
        {
            lobbyClient.Send("clients");
        }
		else
		{
            Debug.Log("Not connected..");
		}
	}

    // for debugging, set id 0
    //private string id = "0";
    public void OnJoinLobbyButtonClicked()
	{
        if(lobbyClient.Connected())
		{
            lobbyClient.Send("enter room");
		}
	}
}
