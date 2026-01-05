using System;
using RoarEngine;

namespace Sandbox
{

    public class Player : RoarEngine.Entity
    {
        private TransformComponent mTransform;
        public float Speed = 1.2f;
        public UInt32 publicID;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");
            publicID = ID;
            mTransform = GetComponent<TransformComponent>();
            //mTransform.Translation = new Vector2(0.0f);
        }

        void OnUpdate(float ts)
        {
            float speed = 15.0f;
            Vector2 velocity = Vector2.Zero;

            if (Input.KeyDown(KeyCode.W))
                velocity.Y = -1.0f;
            else if (Input.KeyDown(KeyCode.S))
                velocity.Y = 1.0f;

            if (Input.KeyDown(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.KeyDown(KeyCode.D))
                velocity.X = 1.0f;

            velocity *= speed;

            Vector2 translation = mTransform.Translation;
            translation += velocity * ts;
            mTransform.Translation = translation;
        }

    }

}
