Push-Location $PSScriptRoot\..\..

foreach ($file in Get-ChildItem -Path .\profiles\win -Filter "*.py" -File)
{
	& "scons" "profile=profiles\win\$($file.name)"
}

Pop-Location