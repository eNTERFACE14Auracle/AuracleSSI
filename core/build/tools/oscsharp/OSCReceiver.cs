using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using Bespoke.Common.Osc;
using System.IO;

namespace oscsharp
{
    public delegate void OscElementHandler(OscPacket element);

    class OSCReceiver
    {
        UdpClient Listener;
        IPEndPoint ipLocal;
        bool running = true;
        public event OscElementHandler Received;        
        private AsyncCallback asyncCallBackDelegate;
        private IAsyncResult asyncResults;

        int port;

        public int Port
        {
            get
            {
                return port;
            }
            set
            {
                port = value;
                if (running == true)
                {
                    stop();
                    start();
                }
            }
        }        

        public OSCReceiver(int port)
        {
            this.port = port;
            this.running = false;
        }

        public void stop()
        {
            System.Console.WriteLine("OSCReceiver: stop listening on port " + port);
                           
            Listener.Close();

            running = false;   
        }

        public void start()
        {
            System.Console.WriteLine("OSCReceiver: start listening on port " + port);

            Listener = new UdpClient(port);
            ipLocal = new IPEndPoint(IPAddress.Any, port);
            asyncCallBackDelegate = new AsyncCallback(OnDataReceived);            
            WaitForData();

            running = true;  
        }
       
        private void WaitForData()
        {
            try
            {
                asyncResults = Listener.BeginReceive(asyncCallBackDelegate, true);
            }
            catch (ObjectDisposedException e)
            {
                Console.WriteLine("Caught: {0}", e.Message);
            }
        }

        private void OnDataReceived(IAsyncResult asyn)
        {           
            if (running == true)
            {
                byte[] bytes;
                try
                {
                    bytes = Listener.EndReceive(asyn, ref ipLocal);
                }
                catch (ArgumentException e)
                {
                    System.Console.WriteLine (e.Message);
                    return;
                }
                catch (ObjectDisposedException oe)
                {
                    System.Console.WriteLine (oe.Message);
                    return;
                }


                byte[] length = BitConverter.GetBytes(bytes.Length);
                byte[] bytes2 = new byte[bytes.Length + length.Length];
                for (int i = 0; i < length.Length; i++)
                {
                    bytes2[i] = length[i];
                }
                for (int i = 0; i < bytes.Length; i++)
                {
                    bytes2[i + length.Length] = bytes[i];
                }
                Stream s = new MemoryStream(bytes2);
                OscPacket element = OscPacket.FromByteArray(ipLocal, bytes);

                if (Received != null)
                {
                    Received(element);
                }

                WaitForData();
            }
            else
            {
                try
                {
                    Listener.EndReceive(asyn, ref ipLocal);
                }
                catch (ArgumentException e)
                {
                    System.Console.WriteLine (e.Message);
                    return;
                }
                catch (ObjectDisposedException oe)
                {
                    System.Console.WriteLine(oe.Message);
                    return;
                }
            }
        }
    }
}
