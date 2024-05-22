#
# This script easy to build win32, linux, winuwp, ios, tvos, osx, android depends on $myRoot/1k/1kiss.ps1
# usage: pwsh build.ps1 -p <targetPlatform> -a <arch>
# options
#  -p: build target platform: win32,winuwp(winrt),linux,android,osx,ios,tvos,watchos
#      for android: will search ndk in sdk_root which is specified by env:ANDROID_HOME first, 
#      if not found, by default will install ndk-r16b or can be specified by option: -cc 'ndk-r23c'
#  -a: build arch: x86,x64,armv7,arm64; for android can be list by ';', i.e: 'arm64;x64'
#  -cc: toolchain: for win32 you can specific -cc clang to use llvm-clang, please install llvm-clang from https://github.com/llvm/llvm-project/releases
#  -xc: additional cmake options: i.e.  -xc '-Dbuild','-DCMAKE_BUILD_TYPE=Release'
#  -xb: additional cross build options: i.e. -xb '--config','Release'
#  -c(configOnly): no build, only generate native project files (vs .sln, xcodeproj)
#  -d: specify project dir to compile, i.e. -d /path/your/project/
#  -f: force generate native project files. Useful if no changes are detected, such as with resource updates.
# examples:
#   - win32:
#     - pwsh build.ps1 -p win32
#     - pwsh build.ps1 -p win32 -cc clang
#   - winuwp: pwsh build.ps1 -p winuwp
#   - linux: pwsh build.ps1 -p linux
#   - android:
#     - pwsh build.ps1 -p android -a arm64
#     - pwsh build.ps1 -p android -a 'arm64;x64'
#   - osx:
#     - pwsh build.ps1 -p osx -a x64
#     - pwsh build.ps1 -p osx -a arm64
#   - ios: pwsh build.ps1 -p ios -a x64
#   - tvos: pwsh build.ps1 -p tvos -a x64
#   - watchos: pwsh build.ps1 -p watchos -a x64
# build.ps1 without any arguments:
# - pwsh build.ps1
#   on windows: target platform is win32, arch=x64
#   on linux: target platform is linux, arch=x64
#   on macos: target platform is osx, arch=x64
#
param(
    [switch]$configOnly,
    [switch]$forceConfig
)

$unhandled_args = @()

$options = @{p = $null; d = $null; xc = @(); xb = @(); }

$optName = $null
foreach ($arg in $args) {
    if (!$optName) {
        if ($arg.StartsWith('-')) {
            $optName = $arg.SubString(1).TrimEnd(':')
        }
        if (!$options.Contains("$optName")) {
            $unhandled_args += $arg
            $optName = $null
            continue
        }
    }
    else {
        if ($options.Contains($optName)) {
            $options[$optName] = $arg
        }
        else {
            $unhandled_args += "-$optName"
            $unhandled_args += $arg
        }
        $optName = $null
    }
}

function translate_array_opt($opt) {
    if ($opt -and $opt -isnot [array]) {
        $opt = "$opt".Split(',')
    }
    return $opt
}

if ($options.xb -isnot [array]) {
    [array]$options.xb = (translate_array_opt $options.xb)
}
if ($options.xc -isnot [array]) {
    [array]$options.xc = (translate_array_opt $options.xc)
}

$myRoot = $PSScriptRoot
$workDir = $(Get-Location).Path

if(Test-Path "$myRoot/1k/1kiss.ps1" -PathType Leaf) {
    $1k_root = $myRoot
}
else {
    throw "The 1k/1kiss.ps1 not found"
}

$source_proj_dir = if($options.d) { $options.d } else { $workDir }

# start construct full cmd line
$1k_script = (Resolve-Path -Path "$1k_root/1k/1kiss.ps1").Path
$1k_args = @()

$search_paths = if ($source_proj_dir -ne $myRoot) { @($source_proj_dir, $myRoot) } else { @($source_proj_dir) }
function search_proj_file($file_path, $type) {
    foreach ($search_path in $search_paths) {
        $full_path = Join-Path $search_path $file_path
        if (Test-Path $full_path -PathType $type) {
            # $ret_path = if ($type -eq 'Container') { $full_path } else { $search_path }
            return $search_path
        }
    }
    return $null
}

$proj_dir = search_proj_file 'CMakeLists.txt' 'Leaf'

$bci = $null # cmake optimize flag param index
# parsing build options
$nopts = $options.xb.Count
for ($i = 0; $i -lt $nopts; ++$i) {
    $optv = $options.xb[$i]
    if($optv -eq '--config') {
        if ($i -lt ($nopts - 1)) {
            $bci = $i + 1
        }
    }
}

if (!$bci) {
    $optimize_flag = 'Release'
    $options.xb += '--config', $optimize_flag
} else {
    $optimize_flag = $options.xb[$bci]
}

if ($proj_dir) {
    $1k_args += '-d', "$proj_dir"
}
$prefix = Join-Path $1k_root 'tools/external'
$1k_args += '-prefix', "$prefix"

# android c++stl
if($options.p -eq 'android' -and !"$($options.xc)".Contains('-DANDROID_STL')) {
    $options.xc += '-DANDROID_STL=c++_shared'
}

# remove arg we don't want forward to
$options.Remove('d')
$1k_args = [System.Collections.ArrayList]$1k_args
foreach ($option in $options.GetEnumerator()) {
    if ($option.Value) {
        $null = $1k_args.Add("-$($option.Key)")
        $null = $1k_args.Add($option.Value)
    }
}

$forward_args = @{}
if ($configOnly) {
    $forward_args['configOnly'] = $true
}
if ($forceConfig) {
    $forward_args['forceConfig'] = $true
}

. $1k_script @1k_args @forward_args @unhandled_args

if (!$configOnly) {
    $1k.pause('Build done')
}
else {
    $1k.pause('Generate done')
}
