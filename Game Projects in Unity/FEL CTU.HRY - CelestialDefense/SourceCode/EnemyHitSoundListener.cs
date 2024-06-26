using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/*
 * Scripta that just calls callback for playing sound
 */
public class EnemyHitSoundListener : MonoBehaviour
{
    public AI_Enemy enemy;

    public void PlayHit()
    {
        enemy.PlayHit();
    }
}
