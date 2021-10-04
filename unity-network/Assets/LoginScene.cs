using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class LoginScene : BaseScene
{
    public LoginManager loginManager;

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

    void LoginCheck()
    {
        if(loginManager.logined)
        {
            // substring login ok
            Debug.Log("My ID is " + loginManager.loginResponse.Substring(8));

            // TODO: set user info based on data from server response

            // TODO: move to lobby scene
            CancelInvoke("LoginCheck");
            SceneManager.LoadScene("LobbyScene");
        }
    }

    public void OnLoginButtonClicked()
    {
        string id = GameObject.Find("IDInputField").GetComponentInChildren<Text>().text;
        string pwd = GameObject.Find("PWDInputField").GetComponentInChildren<Text>().text;

        //Debug.Log("")
        loginManager.Write("login " + id + " " + pwd);

        if (!IsInvoking("loginCheck"))
        {
            InvokeRepeating("LoginCheck", 0.0f, 0.1f);
        }

    }
}
