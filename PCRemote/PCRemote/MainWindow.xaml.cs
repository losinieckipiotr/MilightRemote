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

using Newtonsoft.Json;

namespace PCRemote
{

    public class FrameStruct
    {
        public string[] frame;
        public int resendsNumber;
        public int sequenceLength;
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private byte discoByte = 176;
        private byte ID1Byte = 171;
        private byte ID2Byte = 70;
        private byte colorByte = 0;
        private byte lightByte = 185;
        private byte cmdByte = 0;
        private byte ctrByte = 0;

        private byte holdButton = 0;

        public MainWindow()
        {
            InitializeComponent();
        }

        private string GetJSON()
        {
            try
            {
                string retVal = "";

                string[] frame = new string[7];
                frame[0] = System.Convert.ToString(this.discoByte, 16);
                frame[1] = System.Convert.ToString(this.ID1Byte, 16);
                frame[2] = System.Convert.ToString(this.ID2Byte, 16);
                frame[3] = System.Convert.ToString(this.colorByte, 16);
                frame[4] = System.Convert.ToString(this.lightByte, 16);
                frame[5] = System.Convert.ToString(this.cmdByte, 16);
                frame[6] = System.Convert.ToString(this.ctrByte, 16);

                FrameStruct frameToJSON = new FrameStruct();
                frameToJSON.frame = frame;
                frameToJSON.resendsNumber = System.Convert.ToInt32(resendsBox.Text);
                frameToJSON.sequenceLength = System.Convert.ToInt32(seqBox.Text);

                retVal = JsonConvert.SerializeObject(frameToJSON);

                return retVal;
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

        private void discoSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            try
            {
                int sliderVal = System.Convert.ToInt32(e.NewValue);
                if(sliderVal >= 0 && sliderVal <= 9)
                {
                    string discoByte = "B";
                    discoByte += sliderVal.ToString();
                    discoByteBox.Text = discoByte;
                }
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

                byte typedByte = 0;

                string valueToConvert = txtBox.Text;
                
                if (valueToConvert.Length == 1)
                {
                    valueToConvert = "0" + valueToConvert;
                }
                if(valueToConvert.Length == 2)
                {
                    typedByte = System.Convert.ToByte(valueToConvert, 16);
                }
                else//empty string
                {
                    valueToConvert = "00";
                    typedByte = 0;
                }

                if(txtBox == ID1)
                {
                    if (ID1ByteBox != null)
                    {
                        ID1ByteBox.Text = valueToConvert;
                        this.ID1Byte = typedByte;
                    }
                }
                else if(txtBox == ID2)
                {
                    if (ID2ByteBox != null)
                    {
                        ID2ByteBox.Text = valueToConvert;
                        this.ID2Byte = typedByte;
                    }
                }
                else if(txtBox == colorBox)
                {
                    if(colorByteBox != null)
                    {
                        colorByteBox.Text = valueToConvert;
                        this.colorByte = typedByte;
                        colorSlider.Value = typedByte;
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
                colorBox.Text = hexVal;
                this.discoByte = (byte)val;
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
                this.lightByte = CalcLight(val);
                string hexVal = System.Convert.ToString(this.lightByte, 16).ToUpper();
                if(lightByteBox != null)
                    lightByteBox.Text = hexVal;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                holdButton = 0x10;
                this.cmdByte += 0x10;
                string hexVal = System.Convert.ToString(this.cmdByte, 16).ToUpper();
                if (cmdByteBox != null)
                    cmdByteBox.Text = hexVal;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void RadioButton_Unchecked(object sender, RoutedEventArgs e)
        {
            try
            {
                holdButton = 0x00;
                this.cmdByte -= 0x10;
                string hexVal = System.Convert.ToString(this.cmdByte, 16).ToUpper();
                if (cmdByteBox != null)
                    cmdByteBox.Text = hexVal;
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
                        this.cmdByte = 0x01;
                        break;
                    case "allOffButton":
                        this.cmdByte = 0x02;
                        break;
                    case "changeColorButton":
                        this.cmdByte = 0x0F;
                        break;
                    case "changeLightButton":
                        this.cmdByte = 0x0E;
                        break;
                    case "speedMinusButton":
                        this.cmdByte = 0x0C;
                        break;
                    case "discoModeButton":
                        this.cmdByte = 0x0D;
                        break;
                    case "speedPlusButton":
                        this.cmdByte = 0x0B;
                        break;
                    case "ch1OnButton":
                        this.cmdByte = 0x03;
                        break;
                    case "ch1OffButton":
                        this.cmdByte = 0x04;
                        break;
                    case "ch2OnButton":
                        this.cmdByte = 0x05;
                        break;
                    case "ch2OffButton":
                        this.cmdByte = 0x06;
                        break;
                    case "ch3OnButton":
                        this.cmdByte = 0x07;
                        break;



                    case "ch3OffButton":
                        this.cmdByte = 0x08;
                        break;
                    case "ch4OnButton":
                        this.cmdByte = 0x09;
                        break;
                    case "ch4OffButton":
                        this.cmdByte = 0x10;
                        break;
                    default:
                        MessageBox.Show("Unknown button");
                        return;
                }
                this.cmdByte += holdButton;
                string hexVal = System.Convert.ToString(this.cmdByte, 16).ToUpper();
                cmdByteBox.Text = hexVal;

                string data = this.GetJSON();
                if (data != "")
                {
                    MessageBox.Show(data);
                }

                int seqLength = System.Convert.ToInt32(seqBox.Text);
                seqLength = seqLength % 255;
                this.ctrByte = System.Convert.ToByte(seqLength);
                seqBox.Text = System.Convert.ToString(seqLength, 16).ToUpper();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
           
        }

        private void connectButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }
}
