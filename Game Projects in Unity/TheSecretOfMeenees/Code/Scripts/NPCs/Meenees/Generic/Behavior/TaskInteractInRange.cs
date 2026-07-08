using BehaviorTree;
using UnityEngine;
using UnityEngine.UIElements;

public class TaskInteractInRange : Node
{
    private MeeneeDataSO _data;
    private LayerMask _interactables;
    private Transform _transform;

    private float FOV = 3;

    public TaskInteractInRange(Transform myTransform, LayerMask _whatIsInteract)
    {
        _interactables = _whatIsInteract;
        _transform = myTransform;
    }

    public override NodeState Evaluate()
    {
        object t = GetData("ConversationPartner");
        if(t == null)
        {
            int oldLayer = _transform.gameObject.layer;
            _transform.gameObject.layer = 3;
            Collider[] colliders = Physics.OverlapSphere(
                _transform.position, FOV, _interactables
                );
            _transform.gameObject.layer = oldLayer;
            //remember 1 person it does not want to talk to at a time
            if (colliders.Length > 0 && fuzzyWants((int)GetData("Energy"), 200) && Random.Range(0, 40) == 0 && t != GetData("ConversationNon"))
            {
                //fuzzy wants him, then force him to have me as Conversation Pardner
                if ((colliders[0].gameObject.layer == LayerMask.NameToLayer("NPCG") &&
                    colliders[0].gameObject.GetComponent<MeeneeBT>().WantsToTalk(_transform)) //if we see an NPC and they want to talk
                    ||                                                                      // or
                    (colliders[0].gameObject.layer == LayerMask.NameToLayer("Player"))) //if we see a Player
                {
                    parent.parent.SetData("ConversationPartner", colliders[0].transform);
                    ClearData("ConversationNon");
                    state = NodeState.SUCCESS;
                    return state;
                }
            }
            else if (colliders.Length > 0) parent.parent.SetData("ConversationNon", colliders[0].transform);
            state = NodeState.FAILURE;
            return state;
        }
        state = NodeState.SUCCESS;
        return state;
    }
}
