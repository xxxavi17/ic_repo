# Script de Compilação - DCT Codec
# PowerShell script para compilar o projeto

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   Compilando DCT Audio Codec" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Verificar se CMake está instalado
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Host "ERRO: CMake não encontrado!" -ForegroundColor Red
    Write-Host "Por favor, instale CMake: https://cmake.org/download/" -ForegroundColor Yellow
    exit 1
}

Write-Host "CMake encontrado: $($cmake.Source)" -ForegroundColor Green

# Verificar se há compilador disponível
Write-Host "Procurando compilador C++..." -ForegroundColor Yellow

$compiler = $null
$generator = $null

# Tentar encontrar Visual Studio
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $vsPath = & $vswhere -latest -property installationPath
    if ($vsPath) {
        Write-Host "Visual Studio encontrado: $vsPath" -ForegroundColor Green
        $generator = "Visual Studio 17 2022"  # Ajuste se necessário
        $compiler = "MSVC"
    }
}

# Se não encontrou VS, tentar MinGW
if (-not $compiler) {
    $gcc = Get-Command gcc -ErrorAction SilentlyContinue
    $gxx = Get-Command g++ -ErrorAction SilentlyContinue
    if ($gcc -and $gxx) {
        Write-Host "MinGW encontrado: $($gxx.Source)" -ForegroundColor Green
        $generator = "MinGW Makefiles"
        $compiler = "MinGW"
    }
}

# Se não encontrou nenhum compilador
if (-not $compiler) {
    Write-Host ""
    Write-Host "ERRO: Nenhum compilador C++ encontrado!" -ForegroundColor Red
    Write-Host ""
    Write-Host "Por favor, instale uma das seguintes opções:" -ForegroundColor Yellow
    Write-Host "  1. Visual Studio (recomendado)" -ForegroundColor White
    Write-Host "     https://visualstudio.microsoft.com/downloads/" -ForegroundColor Cyan
    Write-Host "     Durante a instalação, selecione 'Desktop development with C++'" -ForegroundColor White
    Write-Host ""
    Write-Host "  2. MinGW-w64" -ForegroundColor White
    Write-Host "     https://www.mingw-w64.org/downloads/" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "  3. MSYS2 com GCC" -ForegroundColor White
    Write-Host "     https://www.msys2.org/" -ForegroundColor Cyan
    Write-Host "     Depois: pacman -S mingw-w64-x86_64-gcc" -ForegroundColor White
    Write-Host ""
    exit 1
}

Write-Host "Usando compilador: $compiler" -ForegroundColor Green

# Criar diretório de build
$buildDir = "build"
if (Test-Path $buildDir) {
    Write-Host "Limpando build anterior..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}

New-Item -ItemType Directory -Path $buildDir | Out-Null
Write-Host "Diretório build/ criado" -ForegroundColor Green

# Executar CMake
Write-Host ""
Write-Host "Configurando projeto com CMake..." -ForegroundColor Yellow
Set-Location $buildDir

# Configurar com o gerador apropriado
if ($generator) {
    Write-Host "Usando gerador: $generator" -ForegroundColor Cyan
    cmake ..\src -G $generator
} else {
    cmake ..\src
}

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERRO: Falha na configuração do CMake!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host ""
Write-Host "Compilando..." -ForegroundColor Yellow

# Compilar com comando apropriado para o compilador
if ($compiler -eq "MinGW") {
    mingw32-make
} else {
    cmake --build . --config Release
}

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "ERRO: Falha na compilação!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Set-Location ..

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   Compilação concluída com sucesso!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Executáveis criados em: bin/" -ForegroundColor White
Write-Host "  - dct_encoder.exe" -ForegroundColor White
Write-Host "  - dct_decoder.exe" -ForegroundColor White
Write-Host "  - dct_test.exe" -ForegroundColor White
Write-Host ""
Write-Host "Para testar:" -ForegroundColor Yellow
Write-Host "  .\test_codec.ps1" -ForegroundColor White
