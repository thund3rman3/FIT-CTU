using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//borrowed from: https://github.com/MinaPecheux/UnityTutorials-BehaviourTrees


namespace BehaviorTree
{
    public abstract class BTree : MonoBehaviour
    {

        protected Node _root = null;
        private bool seekGuidance;

        protected void Start()
        {
            _root = SetupTree();
            seekGuidance = true;
        }

        private void Update()
        {
            if (_root != null && seekGuidance)
                _root.Evaluate();
        }

        protected abstract Node SetupTree();

        public void ShutItOffOtto()
        {
            seekGuidance = false;
        }
        public void TurnOn()
        {
            seekGuidance = true;
        }
    }

}