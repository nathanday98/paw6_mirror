using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Security.Cryptography.X509Certificates;
using Sharpmake;
using Sharpmake.Generators.JsonCompilationDatabase;

public static class Globals
{
	public static string RootDir;
	public static string CodeRootDir;
	public static string ThirdPartyDir;
	public static string BuildDir;
	public static string ToolsBuildDir;
	public static string TempDir;
	public static string SourceAssetsDir;
	public static string ProjectFilesDir;

	public static Platform[] SupportedPlatforms
	{
		get { return new Platform[] { Platform.win64 }; }
	}

	public static void InitPaths()
	{
		FileInfo file_info = Util.GetCurrentSharpmakeFileInfo();
		Globals.CodeRootDir = Util.SimplifyPath(Path.Combine(file_info.DirectoryName));
		Globals.RootDir = Util.SimplifyPath(Path.Combine(file_info.DirectoryName, ".."));
		Globals.TempDir = Path.Combine(Globals.RootDir, "temp");
		Globals.BuildDir = Path.Combine(Globals.RootDir, "build");
		Globals.ProjectFilesDir = Path.Combine(Globals.CodeRootDir, "project_files");
		Globals.ThirdPartyDir = Path.Combine(Globals.CodeRootDir, "third_party");
		Globals.SourceAssetsDir = Path.Combine(Globals.RootDir, "source_assets");
		Globals.ToolsBuildDir = Path.Combine(Globals.RootDir, "tools_build");
	}

	public static ITarget[] GetTargets()
	{
		Platform TargetPlatforms = 0;
		foreach (Platform platform in Globals.SupportedPlatforms)
		{
			TargetPlatforms |= platform;
		}

		return new[]
		{
			new CustomTarget{
				DevEnv = DevEnv.vs2022,
				Platform = TargetPlatforms,
				Optimization = Optimization.Debug | Optimization.Release | Optimization.Retail,
				BuildSystem = BuildSystem.MSBuild,
				Framework = DotNetFramework.net6_0,
			}
		};
	}

	public static void ApplyLanguageSettings(Project.Configuration conf, CustomTarget target)
	{
		conf.Options.Add(Options.Vc.General.WarningLevel.Level4);
		conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Enable);
		conf.Options.Add(new Options.Vc.Compiler.DisableSpecificWarnings(
			"4514",
			"4710",
			"4820", // padding added to struct
			"5045", // Compiler will insert Spectre mitigation for memory load if / Qspectre switch specified 
			"4701", // potentially uninitialized local variable
			"4703", // potentially uninitialized local pointer variable
			"4062", // enum case not handled in switch
			"4146", // unary minus operator applied to unsigned type, result still unsigned	game
			"4711", // function selected for automatic inline expansion
			"4206", // translation unit is empty,
			"4577", // 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed
			"4626", // assignment operator was implicitly defined as deleted
			"5027", // move assignment operator was implicitly defined as deleted
			"4625", // copy constructor was implicitly defined as deleted
			"5026", // move constructor was implicitly defined as deleted,
			"4530", // C++ exception handler used, but unwind semantics are not enabled.Specify /EHsc
			"4505", // unreferenced function with internal linkage has been removed
			"4201", // nonstandard extension used: nameless struct/union
			"4127" // conditional expression is constant
		));
		conf.Options.Add(Options.Vc.Compiler.Exceptions.Disable);
		conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP20);
		conf.Options.Add(Options.Vc.Compiler.RTTI.Enable);
		conf.Options.Add(Options.Vc.Compiler.ConformanceMode.Enable);
		//conf.Options.Add(Options.Vc.Compiler.FiberSafe.Enable);
		conf.Options.Add(Options.Vc.Compiler.SupportJustMyCode.No);
		// conf.Options.Add(Options.Vc.Compiler.EnableAsan.Enable);

		if (target.Optimization == Optimization.Debug)
		{
			conf.Options.Add(Options.Vc.General.DebugInformation.ProgramDatabaseEnC);
			conf.Options.Add(Options.Vc.Compiler.FunctionLevelLinking.Enable);
			conf.Options.Add(Options.Vc.Linker.Incremental.Enable);
			//conf.Options.Add(Options.Vc.Linker.GenerateFullProgramDatabaseFile.Enable);
			conf.Options.Add(Options.Vc.Compiler.Inline.OnlyInline);
		}

		//// Fucking boost......
		if (target.Optimization == Optimization.Debug)
		{
			conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL);
		}
		else
		{
			conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
		}

		//conf.Options.Add(Options.Vc.General.PlatformToolset.ClangCL);

		//conf.AdditionalCompilerOptions.Add("-Wno-unused-function");
	}
}

public static class ThirdPartyCommon
{
	public static void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		conf.IntermediatePath =
			Path.Combine(Globals.TempDir, "code", "[project.Name]", "[conf.Name]", "[target.Platform]");
		//conf.ProjectPath = "[project.SourceRootPath]";
		conf.ProjectPath = Globals.ProjectFilesDir;

		//// Fucking boost......
		if (target.Optimization == Optimization.Debug)
		{
			conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL);
		}
		else
		{
			conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
		}

		//conf.Options.Add(Options.Vc.General.PlatformToolset.ClangCL);

		// conf.ProjectPath = Globals.ProjectFilesDir;
		// conf.Options.Add(Options.Vc.Compiler.EnableAsan.Enable);
	}
}

[Fragment, Flags]
public enum Optimization
{
	Debug = 1 << 0,
	Release = 1 << 1,
	Retail = 1 << 2,
}

public class CustomTarget : ITarget
{
	public Optimization Optimization;

	public DevEnv DevEnv;

	public Platform Platform;

	public BuildSystem BuildSystem;

	public DotNetFramework Framework;

	public override string Name => Optimization.ToString();

	public override Sharpmake.Optimization GetOptimization()
	{
		switch (Optimization)
		{
			case Optimization.Debug: return Sharpmake.Optimization.Debug;
			case Optimization.Release: return Sharpmake.Optimization.Release;
			case Optimization.Retail: return Sharpmake.Optimization.Retail;
			default: throw new NotSupportedException($"Optimization value {Optimization}");
		}
	}
}

[Generate]
public class STB : Project
{
	public STB() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());

		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "stb/");
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		ThirdPartyCommon.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;

		conf.IncludePaths.Add(Path.Combine(Globals.ThirdPartyDir, "stb/"));
		conf.SolutionFolder = "third_party";

		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
	}
}

[Generate]
public class TinyExr : Project
{
	public TinyExr() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());

		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "tinyexr", "src");
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		ThirdPartyCommon.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;

		conf.IncludePaths.Add(Path.Combine(Globals.ThirdPartyDir, "tinyexr", "include"));
		conf.SolutionFolder = "third_party";
		conf.AddPublicDependency<MiniZ>(target);

		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
	}
}

[Generate]
public class MiniZ : Project
{
	public MiniZ() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());

		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "miniz", "src");
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		ThirdPartyCommon.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;

		conf.IncludePaths.Add(Path.Combine(Globals.ThirdPartyDir, "miniz", "include"));

		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
	}
}

[Generate]
public class LibTess2 : Project
{
	public LibTess2() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());
		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "libtess2", "src");
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		ThirdPartyCommon.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;

		conf.IncludePaths.Add(Path.Combine(Globals.ThirdPartyDir, "libtess2", "include"));
		conf.SolutionFolder = "third_party";
		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
	}
}

[Generate]
public class SGSmooth : Project
{
	public SGSmooth() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());
		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "sgsmooth", "src");
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		ThirdPartyCommon.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;

		conf.IncludePaths.Add(Path.Combine(Globals.ThirdPartyDir, "sgsmooth", "include"));
		conf.SolutionFolder = "third_party";
		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
	}
}

public class PrebuiltProject : Project
{
	protected List<string> dlls;
	protected List<string> libs;
	protected bool apply_configs = false;
	protected List<string> debug_dlls;
	protected List<string>[] optimization_dlls = new List<string>[Enum.GetValues<Optimization>().Length];
	protected List<string>[] optimization_libs = new List<string>[Enum.GetValues<Optimization>().Length];

	public PrebuiltProject() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		string path = Path.Combine(Globals.ThirdPartyDir, this.Name);
		conf.IncludePaths.Add(Path.Combine(path, "include"));

		conf.LibraryPaths.Add(Path.Combine(path, "lib", "[target.Platform]"));
		string dll_path = Path.Combine(path, "bin", "[target.Platform]");

		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
		if (dlls != null)
		{
			foreach (string dll in dlls)
			{
				conf.TargetCopyFiles.Add(Path.Combine(dll_path, dll));
			}
		}

		if (libs != null)
		{
			foreach (string lib in libs)
			{
				conf.LibraryFiles.Add(lib);
			}
		}

		Optimization[] optimization_values = Enum.GetValues<Optimization>();
		for (int i = 0; i < optimization_values.Length; i++)
		{
			if (target.Optimization == optimization_values[i])
			{
				if (optimization_libs[i] != null)
				{
					foreach (string lib in optimization_libs[i])
					{
						conf.LibraryFiles.Add(Path.Combine(target.Optimization.ToString().ToLower(), lib));
					}
				}

				if (optimization_dlls[i] != null)
				{
					foreach (string dll in optimization_dlls[i])
					{
						conf.TargetCopyFiles.Add(Path.Combine(dll_path, target.Optimization.ToString().ToLower(), dll));
					}
				}
			}
		}
	}
}

[Generate]
public class USD : Project
{
	public USD() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());
		Name = "usd";
		NatvisFiles.Add(Path.Combine(Globals.ThirdPartyDir, "usd", "USD.natvis"));
		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "usd", "include");
		SourceFilesExcludeRegex.Add(@"\.*\\(boost)\\");
		SourceFilesExcludeRegex.Add(@"\.*\\(tbb)\\");
	}

	[Configure]
	public void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		string path = Path.Combine(Globals.ThirdPartyDir, this.Name);
		conf.IncludePaths.Add(Path.Combine(path, "include"));
		conf.SolutionFolder = "third_party";
		conf.Output = Configuration.OutputType.None;

		conf.LibraryPaths.Add(Path.Combine(path, "lib", "[target.Platform]"));
		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");

		conf.IntermediatePath =
		Path.Combine(Globals.TempDir, "code", "[project.Name]", "[conf.Name]", "[target.Platform]");
		//conf.ProjectPath = "[project.SourceRootPath]";
		conf.ProjectPath = Globals.ProjectFilesDir;

		string dll_path = Path.Combine(path, "bin", "[target.Platform]");


		string[] dlls = new string[]
		{
			"zlibd.dll",
			"boost_atomic-vc143-mt-gd-x64-1_78.dll",
			"boost_regex-vc143-mt-gd-x64-1_78.dll",
			"tbb.dll",
			"tbb_debug.dll",
			"tbb_preview.dll",
			"tbb_preview_debug.dll",
			"tbbmalloc.dll",
			"tbbmalloc_debug.dll",
			"tbbmalloc_proxy.dll",
			"tbbmalloc_proxy_debug.dll",
			"usd_ms.dll",
		};

		foreach (string dll in dlls)
		{
			conf.TargetCopyFiles.Add(Path.Combine(dll_path, target.Optimization.ToString().ToLower(), dll));
		}

		string[] libs = new string[]
		{
			"boost_atomic-vc143-mt-gd-x64-1_78",
			"boost_regex-vc143-mt-gd-x64-1_78",
			"osdCPU",
			"osdGPU",
			"tbb",
			"tbb_debug",
			"tbb_preview",
			"tbb_preview_debug",
			"tbbmalloc",
			"tbbmalloc_debug",
			"tbbmalloc_proxy",
			"tbbmalloc_proxy_debug",
			"tbbproxy",
			"tbbproxy_debug",
			"usd_ms",
			"zlibd",
		};

		foreach (string lib in libs)
		{
			conf.LibraryFiles.Add(Path.Combine(target.Optimization.ToString().ToLower(), lib));
		}

		string usd_copy_path = Path.Combine(Globals.ThirdPartyDir, "usd", "usd");
		string[] all_usd_copy_files = Directory.GetFiles(usd_copy_path, "*.*", SearchOption.AllDirectories);
		foreach (string file in all_usd_copy_files)
		{
			string final_path = Path.GetDirectoryName(file.Replace(usd_copy_path, "usd"));
			//Console.WriteLine(final_path);
			//Console.WriteLine($"{file} => {Path.Combine(conf.TargetPath, final_path)}");
			conf.TargetCopyFilesToSubDirectory.Add(new KeyValuePair<string, string>(file, final_path));
		}
	}


}


[Generate]
public class Tracy : Project
{
	public Tracy() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());

		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "tracy/");
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		ThirdPartyCommon.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;

		conf.SourceFilesBuildExcludeRegex.Add(".*(?<!TracyClient.cpp)$");
		conf.IncludePaths.Add(Path.Combine(Globals.ThirdPartyDir, "tracy/"));
		conf.SolutionFolder = "third_party";

		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
		conf.ExportDefines.Add("TRACY_ENABLE");
		conf.ExportDefines.Add("TRACY_ON_DEMAND");
		conf.ExportDefines.Add("TRACY_NO_CALLSTACK");
		conf.Defines.Add("TRACY_ENABLE");
		conf.Defines.Add("TRACY_ON_DEMAND");
		conf.Defines.Add("TRACY_NO_CALLSTACK");
	}
}


[Sharpmake.Export]
public class FreeType : PrebuiltProject
{
	public FreeType() : base()
	{
		Name = "FreeType";
		dlls = new List<string>() { "freetype.dll" };
		libs = new List<string>() { "freetype" };
	}
}

[Generate]
public class ImGui : Project
{
	public ImGui() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());

		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "imgui/");
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		ThirdPartyCommon.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;

		conf.IncludePaths.Add(Path.Combine(Globals.ThirdPartyDir, "imgui/"));
		conf.SourceFilesBuildExcludeRegex.Add(@"\.*\\backends\\imgui_impl_(?!dx12|win32)");
		conf.SourceFilesBuildExcludeRegex.Add(@"\.*\\(examples)\\");
		conf.SourceFilesBuildExcludeRegex.Add(@"\.*\\(misc\\fonts)\\");
		conf.SolutionFolder = "third_party";

		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
		conf.Defines.Add("IMGUI_ENABLE_FREETYPE");
		conf.AddPrivateDependency<FreeType>(target);
	}
}


[Generate]
public class ShaderProject : Project
{
	public ShaderProject() : base(typeof(CustomTarget))
	{
		Name = "shaders";
		AddTargets(Globals.GetTargets());
		SourceRootPath = Path.Combine(Globals.RootDir, "source_data", "shaders");
		SourceFilesExtensions.Add(".hlsl");
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
		conf.IntermediatePath = Path.Combine(Globals.TempDir, "code/[project.Name]/[conf.Name]/[target.Platform]");
		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		// conf.ProjectPath = Globals.ProjectFilesDir;
		conf.ProjectPath = "[project.SourceRootPath]";
		conf.Output = Configuration.OutputType.Utility;
	}
}

[Sharpmake.Export]
public class Vulkan : PrebuiltProject
{
	public Vulkan() : base()
	{
		Name = "vulkan";
		libs = new List<string>() { "vulkan-1" };
		dlls = new List<string>() { };
	}
}

[Sharpmake.Export]
public class ENet : PrebuiltProject
{
	public ENet() : base()
	{
		Name = "enet";
		libs = new List<string>() { "enet64", "ws2_32" };
		dlls = new List<string>() { };
	}
}

[Sharpmake.Export]
public class D3D12AgilitySDK : PrebuiltProject
{
	public D3D12AgilitySDK() : base()
	{
		Name = "d3d12_agility_sdk";
		dlls = new List<string>() { "D3D12Core.dll", "d3d12SDKLayers.dll" };
		libs = new List<string>() { };
	}

	public override void ConfigureAll(Configuration conf, CustomTarget target)
	{
		base.ConfigureAll(conf, target);
		conf.LibraryFiles.Add("dxgi.lib");
		conf.LibraryFiles.Add("d3d12.lib");
		conf.LibraryFiles.Add("dxguid.lib");
	}
}

[Sharpmake.Export]
public class WinPixEventRuntime : PrebuiltProject
{
	public WinPixEventRuntime() : base()
	{
		Name = "WinPixEventRuntime";
		dlls = new List<string>() { "WinPixEventRuntime.dll" };
		libs = new List<string>() { "WinPixEventRuntime" };
	}
}

[Sharpmake.Export]
public class DXC : PrebuiltProject
{
	public DXC() : base()
	{
		Name = "dxc";
		dlls = new List<string>() { "dxcompiler.dll", "dxil.dll" };
		libs = new List<string>() { "dxcompiler" };
	}
}

[Generate]
public class SpirVReflect : Project
{
	public SpirVReflect() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());

		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "spirv-reflect/src");
	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		ThirdPartyCommon.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;

		conf.IncludePaths.Add(Path.Combine(Globals.ThirdPartyDir, "spirv-reflect/include"));
		conf.SolutionFolder = "third_party";

		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
	}

}

[Generate]
public class RenderPipelineShaders : Project
{
	public RenderPipelineShaders() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());

		SourceRootPath = Path.Combine(Globals.ThirdPartyDir, "RenderPipelineShaders/src");
		AdditionalSourceRootPaths.Add(Path.Combine(Globals.ThirdPartyDir, "RenderPipelineShaders/include"));

	}

	[Configure]
	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		ThirdPartyCommon.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;

		conf.IncludePrivatePaths.Add("[project.SourceRootPath]");
		conf.IncludePaths.Add(Path.Combine(Globals.ThirdPartyDir, "RenderPipelineShaders/include"));
		conf.SolutionFolder = "third_party";

		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		conf.ExportDefines.Add("RPS_D3D12_RUNTIME");
		conf.Defines.Add("RPS_D3D12_RUNTIME");

		conf.AddPublicDependency<Vulkan>(target);
		conf.AddPrivateDependency<ImGui>(target);
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
	}

}

[Generate]
public class PrebuildToolProject : Project
{
	public PrebuildToolProject() : base(typeof(CustomTarget))
	{
		Name = "prebuild";
		SourceRootPath = Path.Combine(Globals.CodeRootDir, "src", "[project.Name]");
		AddTargets(Globals.GetTargets());
	}

	[Configure]
	public void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		conf.TargetPath = Globals.ToolsBuildDir;
		conf.Output = Configuration.OutputType.Exe;
		conf.IntermediatePath = Path.Combine(Globals.TempDir, "code/[project.Name]/[conf.Name]/[target.Platform]");
		conf.ProjectPath = "[project.SourceRootPath]";

		Globals.ApplyLanguageSettings(conf, target);

		//conf.Options.Add(Options.Vc.Compiler.IgnoreStandardIncludePath.Enable);
	}
}

public class PawProject : Project
{

	public PawProject() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());
		// Enable this when visual studio fixes this bug https://developercommunity.visualstudio.com/t/Go-To-Files-doesnt-show-files-inside-fo/10003783
		CustomProperties.Add("ShowAllFiles", "true");
		SourceRootPath = Path.Combine(Globals.CodeRootDir, "src", "[project.Name]");

		SourceFilesCPPExtensions.Add(".c", ".inl");
	}

	public virtual void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		conf.IntermediatePath = Path.Combine(Globals.TempDir, "code/[project.Name]/[conf.Name]/[target.Platform]");
		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
		//conf.Name = "[target.DevEnv]_[target.Optimization]_[target.Platform]_[target.Framework]";
		conf.ProjectPath = "[project.SourceRootPath]";
		// conf.ProjectPath = Globals.ProjectFilesDir;
		// conf.CustomBuildForAllSources = new Configuration.FileCustomBuild
		// {
		// 	CommandLines = { Path.Combine(Globals.ToolsBuildDir, "prebuild") },
		// 	Outputs = { "[file]" },
		// };

		if (target.Platform == Platform.win64)
		{
			conf.Defines.Add("NOMINMAX");
		}

		conf.Defines.Add($"PAW_TEST_PROJECT_NAME={Name}");


		switch (target.Optimization)
		{
			case Optimization.Debug:
			{
				conf.Defines.Add("PAW_TESTS");
				conf.Defines.Add("PAW_DEBUG");
			}
			break;

			case Optimization.Release:
			{
				conf.Defines.Add("PAW_TESTS");
				conf.Defines.Add("PAW_RELEASE");
			}
			break;

			case Optimization.Retail:
			{
				conf.Defines.Add("PAW_RETAIL");
			}
			break;
		}

		Globals.ApplyLanguageSettings(conf, target);

		if (conf.VcxprojUserFile == null)
		{
			conf.VcxprojUserFile = new Configuration.VcxprojUserFileSettings();
			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = Globals.RootDir;
		}
	}
}

[Generate]
public class TestingProject : PawProject
{
	public TestingProject() : base()
	{
		Name = "testing";
	}

	[Configure]
	public override void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		base.ConfigureAll(conf, target);
		conf.IncludePaths.Add("[project.SourceRootPath]/public");
		conf.Output = Configuration.OutputType.Lib;

	}
}

[Generate]
public class TestAdapterProject : CSharpProject
{
	public TestAdapterProject() : base(typeof(CustomTarget))
	{
		AddTargets(Globals.GetTargets());
		Name = "Pawprint.TestAdapter";
		SourceRootPath = Path.Combine(Globals.CodeRootDir, "src", "test_adapter");
	}

	[Configure]
	public void Configure(CSharpProject.Configuration conf, CustomTarget target)
	{
		conf.Output = Configuration.OutputType.DotNetClassLibrary;
		conf.ProjectPath = "[project.SourceRootPath]";
		conf.IntermediatePath = Path.Combine(Globals.TempDir, "code/[project.Name]/[conf.Name]/[target.Platform]");
		conf.TargetPath = Path.Combine(Globals.BuildDir, "[target.Optimization]/[target.Platform]");
	}
}

[Generate]
public class SharedProject : PawProject
{
	public SharedProject() : base()
	{
		Name = "shared";
	}

	[Configure]
	public override void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		base.ConfigureAll(conf, target);
		conf.IncludePaths.Add("[project.SourceRootPath]/public");
		conf.Output = Configuration.OutputType.Lib;
		conf.AddPublicDependency<STB>(target);
		conf.AddPrivateDependency<TestingProject>(target);
		conf.IncludePaths.Add(Path.Combine(Globals.CodeRootDir, "src", "platform", "public"));
		//conf.Options.Add(Options.Vc.Compiler.IgnoreStandardIncludePath.Enable);

	}
}

[Generate]
public class SimulationProject : PawProject
{
	public SimulationProject() : base()
	{
		Name = "simulation";
	}

	[Configure]
	public override void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		base.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Dll;
		conf.IncludePaths.Add("[project.SourceRootPath]/public");
		conf.AddPublicDependency<SharedProject>(target);
		conf.IncludePaths.Add(Path.Combine(Globals.CodeRootDir, "src", "presentation", "public"));
		conf.Defines.Add("SIMULATION_DLL");
		//conf.Options.Add(Options.Vc.Compiler.IgnoreStandardIncludePath.Enable);
	}
}

[Generate]
public class AssetConvertersProject : PawProject
{
	public AssetConvertersProject() : base()
	{
		Name = "asset_converters";
	}

	[Configure]
	public override void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		base.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;
		conf.IncludePaths.Add("[project.SourceRootPath]/public");
		conf.AddPublicDependency<USD>(target);
	}
}

[Generate]
public class PresentationProject : PawProject
{
	public PresentationProject() : base()
	{
		Name = "presentation";
	}

	[Configure]
	public override void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		base.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;
		conf.IncludePaths.Add("[project.SourceRootPath]/public");

		conf.AddPrivateDependency<SimulationProject>(target);
		//conf.AddPrivateDependency<Optick>(target);
		// conf.AddPublicDependency<EFSW>(target);
		conf.AddPrivateDependency<FreeType>(target);
		conf.AddPublicDependency<ImGui>(target);
		conf.AddPublicDependency<Tracy>(target);
		conf.AddPrivateDependency<Vulkan>(target);
		conf.AddPrivateDependency<SpirVReflect>(target);
		conf.AddPrivateDependency<ENet>(target);
		conf.AddPrivateDependency<LibTess2>(target);
		conf.AddPrivateDependency<USD>(target);
		conf.AddPrivateDependency<TestingProject>(target);
		conf.AddPrivateDependency<SGSmooth>(target);
		conf.AddPrivateDependency<RenderPipelineShaders>(target);
		conf.AddPrivateDependency<RenderGenProject>(target);
		conf.AddPrivateDependency<D3D12AgilitySDK>(target);
		conf.AddPrivateDependency<DXC>(target);
		conf.AddPrivateDependency<WinPixEventRuntime>(target);
		conf.AddPrivateDependency<TinyExr>(target);
		conf.AddPrivateDependency<AssetConvertersProject>(target);

		conf.IncludePaths.Add(Path.Combine(Globals.CodeRootDir, "src", "editor", "public"));
		conf.IncludePaths.Add(Path.Combine(Globals.CodeRootDir, "src", "platform", "public"));
		// conf.AddPrivateDependency<PlatformProject>(target, DependencySetting.IncludePaths);
		conf.IncludePaths.Add("[project.SourceRootPath]/");
		//conf.Options.Add(Options.Vc.Compiler.IgnoreStanda
		//conf.Options.Add(Options.Vc.Compiler.IgnoreStandardIncludePath.Enable);
		conf.Defines.Add("_SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING");
		//conf.ExportDefines.Add("PAW_ENABLE_IMGUI");
		//conf.Defines.Add("PAW_ENABLE_IMGUI");
		conf.Defines.Add("_CRT_SECURE_NO_WARNINGS");
		//conf.Options.Add(Options.Vc.Compiler.FloatingPointModel.Strict);
		//conf.Options.Add(Options.Vc.Compiler.FloatingPointExceptions.Enable);

	}
}

[Generate]
public class RenderGenProject : PawProject
{
	public RenderGenProject() : base()
	{
		Name = "render_gen";
		SourceFilesExtensions.Add(".rpsl");
	}

	[Configure]
	public override void ConfigureAll(Configuration conf, CustomTarget target)
	{
		base.ConfigureAll(conf, target);
		conf.Output = Configuration.OutputType.Lib;
		conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Disable);
	}
}

[Generate]
public class PlatformProject : PawProject
{
	public PlatformProject() : base()
	{
		Name = "platform";
		SourceFilesCPPExtensions.Add(".c");
	}

	[Configure]
	public override void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		base.ConfigureAll(conf, target);
		conf.Options.Add(Options.Vc.Linker.SubSystem.Console);

		conf.Output = Configuration.OutputType.Exe;
		conf.AddPrivateDependency<PresentationProject>(target);
		conf.AddPrivateDependency<Vulkan>(target);
		conf.AddPrivateDependency<TestingProject>(target);
		conf.AddPrivateDependency<D3D12AgilitySDK>(target);
		conf.AddPrivateDependency<SharedProject>(target);

		conf.Defines.Add("_CRT_SECURE_NO_WARNINGS");

		conf.IncludePaths.Add("[project.SourceRootPath]/public");
		conf.LibraryFiles.Add("Shcore.lib");
		conf.LibraryFiles.Add("dxgi.lib");
		conf.LibraryFiles.Add("winmm.lib");
		conf.LibraryFiles.Add("Xinput9_1_0.lib");
		//conf.Options.Add(Options.Vc.Compiler.IgnoreStandardIncl
		//conf.Options.Add(Options.Vc.Compiler.IgnoreStandardIncludePath.Enable

		//List<string> excluded_file_suffixes = new List<string>();
		//foreach (Platform platform in Globals.SupportedPlatforms)
		//{
		//	if (target.Platform != platform)
		//	{
		//		excluded_file_suffixes.Add(platform.ToString());
		//	}
		//}

		//conf.SourceFilesBuildExcludeRegex.Add(@"\.*_(" + string.Join("|", excluded_file_suffixes.ToArray()) + @")\.c$");
		//conf.SourceFilesBuildExcludeRegex.Add(@"\.*_(gl|gl_win64)\.c$");
		//conf.SourceFilesBuildExcludeRegex.Add(@"\.*_(gl|gl_win64)\.h$");
	}
}


[Generate]
public class EditorProject : PawProject
{
	public EditorProject() : base()
	{
		Name = "editor";
		SourceFilesCPPExtensions.Add(".c");
	}

	[Configure]
	public override void ConfigureAll(Project.Configuration conf, CustomTarget target)
	{
		base.ConfigureAll(conf, target);
		conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
		conf.Output = Configuration.OutputType.Exe;
		conf.AddPrivateDependency<ImGui>(target);

		conf.IncludePaths.Add("[project.SourceRootPath]/public");
		conf.LibraryFiles.Add("Shcore.lib");
		conf.LibraryFiles.Add("dxgi.lib");
		conf.LibraryFiles.Add("d3d11.lib");
		conf.LibraryFiles.Add("winmm.lib");
		conf.LibraryFiles.Add("dwmapi.lib");
		conf.LibraryFiles.Add("opengl32.lib");
		conf.Defines.Add("_SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING");
		//conf.AddPrivateDependency<PlatformProject>(target, DependencySetting.OnlyBuildOrder);
		conf.AddPrivateDependency<USD>(target);
		conf.AddPrivateDependency<D3D12AgilitySDK>(target);
		conf.AddPrivateDependency<AssetConvertersProject>(target);

		//conf.Options.Add(Options.Vc.Compiler.IgnoreStandardIncl
		//conf.Options.Add(Options.Vc.Compiler.IgnoreStandardIncludePath.Enable);
	}
}

[Generate]
public class PawSolution : Solution
{
	public PawSolution() : base(typeof(CustomTarget))
	{
		Name = "paw6";

		AddTargets(Globals.GetTargets());
	}

	[Configure]
	public void ConfigureAll(Solution.Configuration conf, CustomTarget target)
	{
		conf.SolutionPath = "[solution.SharpmakeCsPath]";
		conf.PlatformName = target.Platform.ToString();
		conf.AddProject<PlatformProject>(target);
		conf.AddProject<ShaderProject>(target);
		conf.AddProject<EditorProject>(target);
		conf.AddProject<PrebuildToolProject>(target);
		//conf.AddProject<TestAdapterProject>(target);
		//conf.AddProject<TestsProject>(target);
	}
}

public static class Main
{
	[Sharpmake.Main]
	public static void SharpmakeMain(Sharpmake.Arguments arguments)
	{
		arguments.Builder.EventPostSolutionLink += GenerateSolutionDatabase;

		Globals.InitPaths();

		KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2022, KitsRootEnum.KitsRoot10,
			Options.Vc.General.WindowsTargetPlatformVersion.v10_0_22621_0);
		arguments.Generate<PawSolution>();
	}

	private static void GenerateSolutionDatabase(Solution solution)
	{
		var configs = solution.Configurations.SelectMany(c => c.IncludedProjectInfos.Select(pi => pi.Configuration));
		GenerateDatabase(Globals.CodeRootDir, configs, CompileCommandFormat.Command);
	}

	private static void GenerateDatabase(string outdir, IEnumerable<Project.Configuration> configs, CompileCommandFormat format)
	{
		var builder = Builder.Instance;

		if (builder == null)
		{
			System.Console.Error.WriteLine("CompilationDatabase: No builder instance.");
			return;
		}

		var generator = new JsonCompilationDatabase();

		generator.Generate(builder, outdir, configs, format, new List<string>(), new List<string>());
	}
}