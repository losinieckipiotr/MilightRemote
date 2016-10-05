using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.Text.RegularExpressions;
using System.Net;
using System.Net.Sockets;
using System.Threading;

using Newtonsoft.Json;

namespace PCRemote
{

    public class FrameJSON
    {
        public string[] frame;
        public int resendsNumber;
        public int sequenceLength;

        override public string ToString()
        {
            return JsonConvert.SerializeObject(this);
        }
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private string discoByte = "B0";
        private string ID1Byte = "AB";
        private string ID2Byte = "46";
        private string colorByte = "00";
        private string lightByte = "B9";
        private string cmdByte = "00";
        private string ctrByte = "00";

        private bool holdButton = false;
        private bool continuousSend = false;

        private Socket socket = null;

        private bool connected = false;

        public MainWindow()
        {
            InitializeComponent();
        }

        #region backend functions
        private Task TryConnect(IPAddress address, int port)
        {
            Task t = new Task(() =>
            {
                try
                {
                    this.socket = new Socket(
                          AddressFamily.InterNetwork,
                          SocketType.Stream,
                          ProtocolType.Tcp);

                    this.socket.Connect(address, port);
                    connected = true;
                }
                catch (Exception ex)
                {
                    this.socket = null;
                    connected = false;
                    MessageBox.Show("Nie udalo się połączyć: " + ex.Message);
                }
            });
            t.Start();
            return t;
        }

        private Task SendFrame(string frameToSend)
        {
            Task t = new Task(() =>
            {
                try
                {
                    byte[] buffer = ASCIIEncoding.ASCII.GetBytes(frameToSend);
                    this.socket.Send(buffer);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Nie udalo się wysłać ramki: " + ex.Message);
                }
            });
            t.Start();
            return t;
        }

        private string GetJSON()
        {
            try
            {
                string[] frame = new string[7];
                frame[0] = this.discoByte;
                frame[1] = this.ID1Byte;
                frame[2] = this.ID2Byte;
                frame[3] = this.colorByte;
                frame[4] = this.lightByte;
                frame[5] = this.cmdByte;
                frame[6] = this.ctrByte;

                FrameJSON frameToSend = new FrameJSON();
                frameToSend.frame = frame;
                if (resendsBox.Text == "")
                    frameToSend.resendsNumber = 0;
                else
                    frameToSend.resendsNumber = System.Convert.ToInt32(resendsBox.Text);
                if (seqBox.Text == "")
                    frameToSend.sequenceLength = 0;
                else
                    frameToSend.sequenceLength = System.Convert.ToInt32(seqBox.Text);

                return frameToSend.ToString();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return "";
            }
        }

        private byte CalcLight(int lightInPerc)
        {
            if (lightInPerc >= 100)
                return (byte)(0xB9);
            else if (lightInPerc == 0)
                return (byte)(0x81);

            int t = (int)(Math.Round(lightInPerc / 4.0f));
            int v = 0xFF - 0x7E - t * 8;
            if (v < 0)
                v = 0xFF + 0x82 - t * 8;
            return (byte)v;
        }

        private void UpdateFrameCounter()
        {
            int seqLength = System.Convert.ToInt32(seqBox.Text);
            int ctr = System.Convert.ToInt32(this.ctrByte, 16) + seqLength;
            ctr = ctr % 256;
            string ctrStr = System.Convert.ToString(ctr, 16).ToUpper();
            if (ctrStr.Length == 1)
                ctrStr = "0" + ctrStr;
            this.ctrByte = ctrByteBox.Text = ctrBox.Text = ctrStr;
        }

        private async void SendFrameIfNeeded(bool remoteClick = false)
        {
            if (connected)
            {
                if (remoteClick || continuousSend)
                {
                    string frame = this.GetJSON();
                    if (frame != "")
                    {
                        //MessageBox.Show(frame);

                        await SendFrame(frame);
                        UpdateFrameCounter();
                    }
                }
            }
        }

        #endregion

        #region remoteControls
        private void discoSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            try
            {
                int sliderVal = System.Convert.ToInt32(e.NewValue);
                this.discoByte = "B" + sliderVal.ToString();
                discoByteBox.Text = this.discoByte;

                SendFrameIfNeeded();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void bytePreview_TextInput(object sender, TextCompositionEventArgs e)
        {
            try
            {
                Regex regex = new Regex("[^0-9A-F]");
                bool ok = regex.IsMatch(e.Text);
                TextBox box = (TextBox)sender;
                if (box.Text.Length == 2)
                    ok = true;
                e.Handled = ok;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void byteBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                TextBox txtBox = (TextBox)sender;
                if (txtBox == null)
                    return;

                string typedByte = txtBox.Text;
                
                if (typedByte.Length == 1)
                {
                    typedByte = "0" + typedByte;
                }
                else if(typedByte == "")
                {
                    typedByte = "00";
                }

                if(txtBox == ID1)
                {
                    if (ID1ByteBox != null)
                    {
                        this.ID1Byte = ID1ByteBox.Text = typedByte;
                    }
                }
                else if(txtBox == ID2)
                {
                    if (ID2ByteBox != null)
                    {
                        this.ID2Byte = ID2ByteBox.Text = typedByte;
                    }
                }
                else if(txtBox == colorBox)
                {
                    if(colorByteBox != null)
                    {
                        this.colorByte = colorByteBox.Text = typedByte;
                        colorSlider.Value = System.Convert.ToInt32(typedByte, 16);
                    }
                }
                else if(txtBox == ctrBox)
                {
                    if(ctrByteBox != null)
                    {
                        this.ctrByte = ctrByteBox.Text = typedByte;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void colorSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            try
            {
                int val = System.Convert.ToInt32(e.NewValue);
                string hexVal = System.Convert.ToString(val, 16).ToUpper();
                this.colorByte = colorBox.Text = hexVal;

                SendFrameIfNeeded();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void lightSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            try
            {
                int val = System.Convert.ToInt32(e.NewValue);
                val = CalcLight(val);
                string hexVal = System.Convert.ToString(val, 16).ToUpper();
                if(lightByteBox != null)
                    this.lightByte = lightByteBox.Text = hexVal;

                SendFrameIfNeeded();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void holdButton_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                holdButton = true;
                this.cmdByte = "1" + this.cmdByte.Substring(1, 1);
                if (cmdByteBox != null)
                    cmdByteBox.Text = this.cmdByte;

                SendFrameIfNeeded();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void holdButton__Unchecked(object sender, RoutedEventArgs e)
        {
            try
            {
                holdButton = false;
                this.cmdByte = "0" + this.cmdByte.Substring(1, 1);
                if (cmdByteBox != null)
                    cmdByteBox.Text = this.cmdByte;

                SendFrameIfNeeded();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void remoteButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Button buttSender = (Button)sender;

                switch (buttSender.Name)
                {
                    case "allOnButton":
                        this.cmdByte = "01";
                        break;
                    case "allOffButton":
                        this.cmdByte = "02";
                        break;
                    case "changeColorButton":
                        this.cmdByte = "0F";
                        break;
                    case "changeLightButton":
                        this.cmdByte = "0E";
                        break;
                    case "speedMinusButton":
                        this.cmdByte = "0C";
                        break;
                    case "discoModeButton":
                        this.cmdByte = "0D";
                        break;
                    case "speedPlusButton":
                        this.cmdByte = "0B";
                        break;
                    case "ch1OnButton":
                        this.cmdByte = "03";
                        break;
                    case "ch1OffButton":
                        this.cmdByte = "04";
                        break;
                    case "ch2OnButton":
                        this.cmdByte = "05";
                        break;
                    case "ch2OffButton":
                        this.cmdByte = "06";
                        break;
                    case "ch3OnButton":
                        this.cmdByte = "07";
                        break;
                    case "ch3OffButton":
                        this.cmdByte = "08";
                        break;
                    case "ch4OnButton":
                        this.cmdByte = "09";
                        break;
                    case "ch4OffButton":
                        this.cmdByte = "0A";
                        break;
                    default:
                        MessageBox.Show("Unknown button");
                        return;
                }
                if(holdButton)
                    this.cmdByte = "1" + this.cmdByte.Substring(1, 1);
                cmdByteBox.Text = this.cmdByte;

                SendFrameIfNeeded(true);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        #endregion

        #region appControl
        private void continuousSend_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                continuousSend = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void continuousSend__Unchecked(object sender, RoutedEventArgs e)
        {
            try
            {
                continuousSend = false;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void numberBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            try
            {
                Regex regex = new Regex("[^0-9]");
                bool ok = regex.IsMatch(e.Text);
                TextBox box = (TextBox)sender;
                if (box.Text.Length > 4)
                    ok = true;
                e.Handled = ok;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void numberBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                TextBox txtBox = (TextBox)(sender);
                string txt = txtBox.Text;

                //seqBox
                if (txtBox == seqBox)
                {
                    if (txt == "" || txt == "0")
                    {
                        txtBox.Text = "1";
                        return;
                    }
                }
                //resendsBox
                else if(txt == "")
                {
                    return;
                }

                int val = System.Convert.ToInt32(txt);
                if (val > 1000)
                {
                    txtBox.Text = "1000";
                    txtBox.CaretIndex = 4;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void ipBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            try
            {
                Regex regex = new Regex("[^0-9.]");
                bool ok = regex.IsMatch(e.Text);
                TextBox box = (TextBox)sender;
                if (box.Text.Length == 15)
                    ok = true;
                e.Handled = ok;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void portBox_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            try
            {
                Regex regex = new Regex("[^0-9]");
                bool ok = regex.IsMatch(e.Text);
                TextBox box = (TextBox)sender;
                if (box.Text.Length == 4)
                    ok = true;
                e.Handled = ok;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private async void connectButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!connected)
                {
                    string IPString = ipBox.Text;
                    IPAddress address = null;
                    if (!IPAddress.TryParse(IPString, out address))
                        return;

                    Int16 port = 2016;
                    if (!Int16.TryParse(portBox.Text, out port))
                        return;

                    await TryConnect(address, port);
                    if (connected)
                    {
                        connectButton.Content = "Disconnect";
                    }
                }
                else
                {
                    socket.Close();
                    socket = null;
                    connected = false;
                    connectButton.Content = "Connect";
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
        #endregion
    }
}
