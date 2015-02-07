using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Bespoke.Common.Osc;

namespace oscsharp
{
    class Program
    {
        static void Main(string[] args)
        {

            int port = 1234;
            if (args.Length > 0) {
                port = Int32.Parse(args[0]);
            }

            OSCReceiver receiver = new OSCReceiver(port);
            receiver.Received += received;

            receiver.start();

            System.Console.WriteLine("press enter to quit");
            Console.Read();

            receiver.stop();
        }

        static public void received(OscPacket element)
        {
            switch (element.Address)
            {
                case "/text":
                    recv_message((String)element.Data[0], (float)element.Data[1], (String)element.Data[2]);
                    break;
                case "/evnt":
                    int num = (int)element.Data[3];
                    String[] names = new String[num];
                    float[] values = new float[num];
                    int j = 4;
                    for (int i = 0; i < num; i++)
                    {
                        names[i] = (String)element.Data[j++];
                        values[i] = (float)element.Data[j++];
                    }
                    recv_event((String)element.Data[0], (float)element.Data[1], (float)element.Data[2], num, names, values);
                    break;
                case "/strm":
                    recv_stream((String)element.Data[0], (float)element.Data[1], (float)element.Data[2], (int)element.Data[3], (int)element.Data[4], (int)element.Data[5], (int)element.Data[6], (byte[])element.Data[7]);
                    break;
            }
        }

        public static void recv_message(String id, float time, String msg)
        {
            System.Console.WriteLine("text <" + id + "@" + time + "s>");
        }

        public static void recv_event(String id, float time, float dur, int num, String[] event_names, float[] event_values)
        {
            System.Console.WriteLine("evnt <" + id + "@" + time + "s>");
        }

        public static void recv_stream(String id, float time, float sr, int num, int dim, int bytes, int type, byte[] data)
        {
            System.Console.WriteLine("strm <" + id + "@" + time + "s>");
        }

    }
}
