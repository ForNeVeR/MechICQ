using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace MechICQ
{
    public partial class Form1 : Form
    {
        ICQProtocol oscar;
        Thread messageThread;

        public Form1()
        {
            InitializeComponent();
            oscar = new ICQProtocol();
            messageThread = new Thread(messageQueueManager);
            messageThread.Start();
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (comboBox1.SelectedIndex)
            {
                case 0:
                    // TODO: Отключение
                    break;
                case 1:
                    // TODO: Подключение
                    oscar.StartConnect(100000000, "password");
                    break;
            }
        }

        private void messageQueueManager()
        {
            while (true)
            {
                // Проверяем случай рассинхронизации, когда в очереди задач что-то есть, а семафор нас не пускает
                lock (oscar.generalMessages)
                {
                    if (oscar.generalMessages.Count > 0)
                        oscar.messageReady.Set();
                }
                // Ждём пока нам скажут, что задачи готовы
                oscar.messageReady.WaitOne();
                // Цикл обработки задач
                while (oscar.generalMessages.Count > 0)
                {
                    GeneralMessage tmp_message;
                    lock (oscar.generalMessages)
                    {
                        tmp_message = oscar.generalMessages.Dequeue();
                    }
                    switch (tmp_message.getMessageType())
                    {
                        case GeneralMessageType.ConnectFailed:
                            ConnectionErrorMessage msg = (ConnectionErrorMessage)tmp_message;
                            MessageBox.Show("Ошибка при подключении: \n" + msg.getMessageString());
                            break;
                        default:
                            MessageBox.Show("Обнаружено необрабатываемое сообщение модуля протокола!");
                            break;
                    }
                }
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            messageThread.Abort();
        }
    }
}
