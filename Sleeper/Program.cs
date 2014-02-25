using System;
using System.Threading;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Sleeper
{
    class Program
    {
        static CallbackHandler _callback = new CallbackHandler(MyCallbackFunction);

        static void Main(string[] args)
        {
            //Console.WriteLine(Thread.CurrentThread.GetApartmentState());

            //foreach (Assembly assy in AppDomain.CurrentDomain.GetAssemblies())
            //{
            //    Console.WriteLine(assy.FullName + "\t" + assy.Location);
            //}

            RegisterThreadTestCallback(Marshal.GetFunctionPointerForDelegate(_callback));
            InvokeThreadTestCallback();
            InvokeThreadTestCallbackThreaded();

            Console.WriteLine("starting");

            for (int i = 0; !Console.KeyAvailable; i++)
            {
                Console.WriteLine(i);
                Thread.Sleep(1000);
            }

            Console.WriteLine("exiting");
        }

        private static void MyCallbackFunction()
        {
            Console.WriteLine("hello, thread!");
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void RegisterThreadTestCallback(IntPtr callback);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void InvokeThreadTestCallback();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void InvokeThreadTestCallbackThreaded();
    }

    public delegate void CallbackHandler();
}
