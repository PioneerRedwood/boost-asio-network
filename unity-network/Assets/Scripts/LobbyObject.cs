using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LobbyObject : MonoBehaviour
{
    /*
     * 로비 프리팹
     *  1차 이름, 현재 인원수, 최대수, 
     *  2차 현 유저
     *  
     *  lobby|index,name,current_count,max_count|index,name,current,max|
     */
    [SerializeField]
    private Text index;

    [SerializeField]
    private Text lobbyName;

    [SerializeField]
    private Text userCount;

	private void Start()
	{
		
	}

	private void Update()
	{
		
	}

    public void SetData(string index_, string lobbyName_, string userCount_)
	{
        index.text = index_;
        lobbyName.text = lobbyName_;
        userCount.text = userCount_;
	}
}