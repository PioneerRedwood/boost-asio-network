using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class LoginScene : BaseScene
{
    void Awake()
    {
        sceneType = RedNetwork.SceneType.Login;


    }

    void Start()
    {
        
    }

    void Update()
    {
        
    }

    void OnLoginButtonClicked()
    {
        NetworkManager networkManager = GameObject.Find("NetworkManager").GetComponent<NetworkManager>();
        string id = "red";
        string pwd = "pwd";

        networkManager.Write("login " + id + " " + pwd);
    }
}
