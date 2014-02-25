using System;
using System.Threading;
using System.Reflection;

namespace Sleeper
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine(Thread.CurrentThread.GetApartmentState());

            foreach (Assembly assy in AppDomain.CurrentDomain.GetAssemblies())
            {
                Console.WriteLine(assy.FullName + "\t" + assy.Location);
            }

            Console.WriteLine("starting");
            Thread.Sleep(10000);
            Console.WriteLine("exiting");
        }
    }
}
