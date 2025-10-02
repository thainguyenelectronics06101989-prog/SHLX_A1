using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerSHLX
{
    internal class ModelTest
    {
        private int a;
    }
    class Motor
    {
        private const int numOfmagnet = 8;
        private const int numOftest = 4;
        private byte MotorID {  get; }
        private int[] distTest {  get; set; }
        private bool[] magnetCounter { get; set; }
        private bool engine {  get; set; }
        private bool signedL {  get; set; }
        private bool signedR { get; set; }
        private int speed { get; set; }


        public Motor(byte id)
        {
            MotorID = id;
            distTest = new int[numOftest];
            for (int i = 0; i < numOftest; i++)
            {
                distTest[i] = 0;
            }
            magnetCounter = new bool[numOfmagnet];
            for (int i = 0; i < numOfmagnet; i++)
            {
                magnetCounter[i] = false;
            }
        }


    }
    class Yard
    {
        private bool[] airSensor { get; set; }
        public Yard() { }

    }
}
