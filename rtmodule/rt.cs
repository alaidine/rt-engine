using System;

namespace RTEngine
{

    public class Main
    {
        public float FloatVar { get; set; }

        public Main()
        {
            Console.WriteLine("Main Contructor!!!!");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello world from C#!");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine(message);
        }
    }

}
