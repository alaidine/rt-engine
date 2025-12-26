using System;
using System.Runtime.CompilerServices;

namespace RTEngine
{

    public struct Vector2
    {
        public float X, Y;

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }
    }

    public class Main
    {
        public float FloatVar { get; set; }

        public Main()
        {
            Console.WriteLine("Main Contructor!!!!");
            Log("Testing native log", 42069);

            Vector2 pos = new Vector2(1, 2.3f);
            Vector2 result = Log(pos);
            Console.WriteLine($"{result.X} {result.Y}");

            float dot = NativeLogVectorDot(ref pos);
            Console.WriteLine(dot);
        }

        public void PrintInt(int value)
        {
            Console.WriteLine($"C# says: {value}");
        }

        public void PrintInts(int value1, int value2)
        {
            Console.WriteLine($"C# says: {value1} and {value2}");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello world from C#!");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }

        public void Log(string message, int parameter)
        {
            NativeLog(message, parameter);
        }

        public Vector2 Log(Vector2 vector)
        {
            NativeLogVector2(ref vector, out Vector2 vec);
            return vec;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static void NativeLog(string message, int parameter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static void NativeLogVector2(ref Vector2 param, out Vector2 vec);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern static float NativeLogVectorDot(ref Vector2 vec);


    }

}
