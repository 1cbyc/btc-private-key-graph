// Created by Nsisong

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace NsisongBTCkeyGen
{
    class Program
    {
        static string generatePrivKey(int length)
        {
            const string validChars = "QWERTYUIOPASDFGHJKLZXCVBNM1234567890qwertyuiopasdfghjklzxcvbnm";
            StringBuilder res = new StringBuilder();
            Random rnd = new Random();
            while (0 < length--)
            {
                res.Append(validChars[rnd.Next(validChars.Length)]);
            }
            return res.ToString();
        }
        
        static void Main(string[] args)
        {
            //password length
            int length = 2;
            
            string pass = generatePrivKey(length);
            Console.Write("KwoYj1SHyiv3zf2KMK7sxS7YGRidKJaXeiCa8Tb8rVU61Mivtr" + pass);
        }
    }
}
