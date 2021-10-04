using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

namespace RedNetwork
{
    public enum SceneType
    {
        Default = 0,
        Login = 1,
        Lobby = 2,
        Game = 3,
    }
}

public abstract class BaseScene : MonoBehaviour
{
    public RedNetwork.SceneType sceneType = RedNetwork.SceneType.Default;

    void Awake()
    {

    }
}
