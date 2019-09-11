
$cmake_found=$(where.exe cmake)
if (!$cmake_found) {
  echo "Cmake was not found in the PATH. Install Visual Studio's latest release with CMake support and add the cmake binary to the PATH."
  echo "You can find the location of your Visual Studio's CMake instalation by opening the Visual Studio's prompt and using ""where cmake"""
  return ;
}

& "${cmake_found}" "-G""Visual Studio 16 2019""" -H"".."" -B""./vs""
