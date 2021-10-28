using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LobbyUIManager : Singleton<LobbyUIManager>
{
    class Lobby
    {
        /*
         * 로비.
         *  1차 이름, 현재 인원수, 최대수, 
         *  2차 현 유저
         *  
         *  lobby|index,name,current_count,max_count|index,name,current,max|
         */

        public int index;
        public string name;
        public int currentCount;
        public int maxCount;


    }

    public bool CreateLobbies(string msg)
    {
        string[] res = msg.Split('|');
        foreach (string lob in res)
        {
            string[] temp = lob.Split(',');

            if (temp.Length == 4)
            {
                Lobby lobby = new Lobby()
                {
                    index = int.Parse(temp[0]),
                    name = temp[1],
                    currentCount = int.Parse(temp[2]),
                    maxCount = int.Parse(temp[3])
                };
                lobbies.Add(lobby);
            }
        }

        if (lobbies.Count > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    [SerializeField]
    private List<Lobby> lobbies = new List<Lobby>();

    [SerializeField]
    private GameObject lobbyObject;

    [SerializeField]
    private Transform contentParentTransform;

    // Start is called before the first frame update
    void Start()
    {
		
	}

    // Update is called once per frame
    void Update()
    {
        
    }

    public void InitUI()
	{
		foreach (Lobby lobby in lobbies)
		//for(int i = 0; i < 8; ++i)
		{
            GameObject obj = Instantiate(lobbyObject, Vector3.zero, Quaternion.identity);
            obj.transform.SetParent(contentParentTransform);
		}
	}
}
