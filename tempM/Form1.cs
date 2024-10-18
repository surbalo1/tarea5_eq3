using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;

namespace tempM
{
    public partial class Form1 : Form
    {
        

        public Form1()
        {
            InitializeComponent();
        }

        private void lbl_a1_Click(object sender, EventArgs e)
        {

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            pBof.Image = tempM.Properties.Resources.apagado;
            try
            {

                string[] portlist = SerialPort.GetPortNames();
                comboBoxPort.Items.AddRange(portlist);

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void btnOpen_Click(object sender, EventArgs e)
        {
            try
            {
                serialPort1.PortName = comboBoxPort.Text;
                serialPort1.BaudRate = Convert.ToInt32(comboBoxBaud.Text);
                serialPort1.Open();
                timer1.Enabled = true;
                
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            try
            {
                timer1.Enabled = false;
                serialPort1.Close();
                lbl_a1.Text = "---";
                lbl_a2.Text = "---";
                lbl_a3.Text = "---";
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        char[] delimitador = { ',' };
        string[] temps;
        string entrada;
        string estacion;
        private void timer1_Tick(object sender, EventArgs e)
        {
            serialPort1.Write("l");

            entrada = serialPort1.ReadLine();
            temps = entrada.Split(delimitador);
 
            if (temps[3] == "0")
            {
                pBof.Image = tempM.Properties.Resources.apagado;
            }
            else if (temps[3] == "1") {
                pBof.Image = tempM.Properties.Resources.encendido;
            }

            if (temps[0] == "0.0" || temps[0] == "40.0")
                 lbl_a1.Text = "---";
            else
                lbl_a1.Text = temps[0] + "°C";

            if (temps[1] == "0.0" || temps[1] == "40.0")
                lbl_a2.Text = "---";
            else
                lbl_a2.Text = temps[1] + "°C";

            if (temps[2] == "0.0" || temps[2] == "40.0")
                lbl_a3.Text = "---";
            else
                 lbl_a3.Text = temps[2] + "°C";

            estacion = temps[4];

            if (estacion == "1") lbl_temporada.Text = "INVIERNO";
            if (estacion == "2") lbl_temporada.Text = "OTOÑO-PRIMAVERA";
            if (estacion == "3") lbl_temporada.Text = "VERANO";

        }
    }
}
