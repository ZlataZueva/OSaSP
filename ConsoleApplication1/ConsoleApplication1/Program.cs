using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            int[] arr = new int[] { 1, 2, 3, 4, 5, 6, 7, 8, 9, 100 };
            var newarr = arr
                .AsParallel()
                .WithDegreeOfParallelism(4)
                .Where(n => n > 100)
                .Select(n => n * n).ToArray();

            System.Console.WriteLine(newarr);
            System.Console.ReadKey();
        }
    }
}
