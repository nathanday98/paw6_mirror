using Microsoft.VisualStudio.TestPlatform.ObjectModel;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Adapter;
using Microsoft.VisualStudio.TestPlatform.ObjectModel.Logging;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Pawprint.TestAdapter
{
	[FileExtension(".exe")]
	[DefaultExecutorUri(TestExecutir.ExecutorUriString)]
	public class TestDiscoverer : ITestDiscoverer
	{
		public void DiscoverTests(IEnumerable<string> sources, IDiscoveryContext discoveryContext, IMessageLogger logger, ITestCaseDiscoverySink discoverySink)
		{
			int test_case_count = 0;
			foreach (string source in sources)
			{

				Process process = new Process();
				process.StartInfo.CreateNoWindow = true;
				process.StartInfo.RedirectStandardOutput = true;
				process.StartInfo.UseShellExecute = false;
				process.StartInfo.FileName = source;
				process.StartInfo.Arguments = "/list_all_tests";

				logger.SendMessage(TestMessageLevel.Informational, $"Starting {source} {process.StartInfo.Arguments}");


				if (process.Start())
				{
					StreamReader reader = process.StandardOutput;
					do
					{
						string line = reader.ReadLine();
						string[] sections = line.Split(new string[] { " - " }, StringSplitOptions.RemoveEmptyEntries);
						string[] names = sections[0].Split(new string[] { "::" }, StringSplitOptions.RemoveEmptyEntries);
						string[] paths = sections[1].Split(new string[] { "::" }, StringSplitOptions.RemoveEmptyEntries);
						string path = paths[0];
						int line_number = int.Parse(paths[1]);
						discoverySink.SendTestCase(new TestCase
						{
							CodeFilePath = path,
							LineNumber = line_number,
							ExecutorUri = TestExecutir.ExecutorURI,
							FullyQualifiedName = sections[0],
							Source = source,
						});

						test_case_count++;
					} while (!reader.EndOfStream);
				}
			}
			logger.SendMessage(TestMessageLevel.Informational, $"Discovered {test_case_count} tests");
		}

	}


	[ExtensionUri(ExecutorUriString)]
	public class TestExecutir : ITestExecutor
	{
		public const string ExecutorUriString = "executor://PawprintExecutor";
		public static readonly Uri ExecutorURI = new Uri(ExecutorUriString);

		public void Cancel()
		{

		}

		public void RunTests(IEnumerable<TestCase> tests, IRunContext runContext, IFrameworkHandle frameworkHandle)
		{

		}

		public void RunTests(IEnumerable<string> sources, IRunContext runContext, IFrameworkHandle frameworkHandle)
		{
			foreach ()
		}
	}
}
