using System;
using System.Globalization;
using System.IO.Ports;
using System.Speech.Synthesis;

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
            //syntezier.Speak("Ob du wirklich richtig stehst, siehst du wenn das Licht angeht!");

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
            sp.NewLine = "\r\n";
            sp.Open();
            sp.DataReceived += Sp_DataReceived;

            while (true)
            {
                string output = Console.ReadLine();
                Console.WriteLine(output);
                sp.WriteLine(output);
            }
        }

        private static void Sp_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            SerialPort serialPort = (SerialPort)sender;
            if (serialPort.IsOpen)
            {
                // byte[] bytes = new byte[serialPort.BytesToRead];
                // serialPort.Read(bytes, 0, serialPort.BytesToRead);
                // string text = System.Text.Encoding.Default.GetString(bytes);
                string text = serialPort.ReadLine();

                Console.WriteLine(text);
                syntezier.Speak(text);
            }
        }
    }
}
