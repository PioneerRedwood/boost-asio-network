using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class NetworkManager : MonoBehaviour
{
    [SerializeField]
    private Dictionary<string, string> userInfo = new Dictionary<string, string>();

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

	// Start is called before the first frame update
	void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        
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
                // Unity는 스레드가 안된다? -- 이에 대해 다시 찾아봐야 함
                GameObject.Find("IDText").GetComponent<Text>().text = userInfo["id"];
                RedNetwork.LobbyClient lobbyClient = new RedNetwork.LobbyClient();
                lobbyClient.Connect("127.0.0.1", 9000);
                InvokeRepeating(nameof(lobbyClient.PingToServer), 3.0f, 1.0f);
            }
            catch (Exception e)
            {
                Debug.Log(e);
            }
        }
    }
}
