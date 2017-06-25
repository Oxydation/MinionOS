using System;
using System.Globalization;
using System.IO.Ports;
using System.Speech.Synthesis;
using System.Text;
using System.Threading;

namespace SerialTTS
{
    class Program
    {
        static SerialPort sp;
        static SpeechSynthesizer syntezier = new SpeechSynthesizer();
        static void InitSpeecher()
        {
            syntezier.Volume = 100;
            syntezier.Rate = 0;
            var bla = syntezier.GetInstalledVoices(CultureInfo.GetCultureInfo("de-DE"));
            syntezier.SelectVoice(bla[0].VoiceInfo.Name);
        }
        static void Main(string[] args)
        {
            InitSpeecher();

            Console.WriteLine("Serial TTS v1.0");
            foreach (var portName in SerialPort.GetPortNames())
            {
                Console.Write(portName + ", ");
            }
            Console.WriteLine();
            Console.Write("Please enter a valid com port (e.g. COM5): ");

            string port = Console.ReadLine();
            if (!port.Contains("COM"))
            {
                port = "COM5";
            }
            sp = new SerialPort(port);

            sp.BaudRate = 115200;
            sp.StopBits = StopBits.One;
            sp.DataBits = 8;
            sp.Parity = Parity.None;
            sp.NewLine = "\r";
           // sp.DataReceived += Sp_DataReceived;
            Console.InputEncoding = Encoding.UTF8;
            Console.OutputEncoding = Encoding.UTF8;
            sp.Open();
            
            Console.CancelKeyPress += Console_CancelKeyPress;
            Thread thread = new Thread(() =>
            {
                while (true)
                {
                    string output = Console.ReadLine();
                    //Console.WriteLine(output);
                    sp.WriteLine(output);
                }
            });
            thread.Start();

            while (true)
            {
                string text = sp.ReadLine();

                Console.WriteLine(text);
                syntezier.SpeakAsync(text);
            }
            Console.ReadLine();
        }

        private static void Console_CancelKeyPress(object sender, ConsoleCancelEventArgs e)
        {
            e.Cancel = true;
        }

        private static void Sp_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort serialPort = (SerialPort)sender;
            if (serialPort.IsOpen)
            {
                string text = serialPort.ReadLine();

                if(text.Contains("\n") || text.Contains("\r"))
                {

                    Console.Write(text);
                }
                else
                {
                    Console.WriteLine(text);
                }
                syntezier.SpeakAsync(text);
            }
        }
    }
}
