program gcc_check;

{$mode objfpc}{$H+}

uses
  SysUtils, Process;

var
  OutputStr, SubStr: string;
  MajorVer, MinorVer, DotPos: Integer;

begin
  if not RunCommand('gcc', ['-dumpfullversion', '-dumpversion'], OutputStr) then
  begin
    writeln(StdErr, 'gcc-check: error: gcc not found in PATH');
    Halt(1);
  end;

  OutputStr := Trim(OutputStr);
  DotPos := Pos('.', OutputStr);

  if DotPos > 0 then
  begin
    MajorVer := StrToIntDef(Copy(OutputStr, 1, DotPos - 1), 0);
    SubStr := Copy(OutputStr, DotPos + 1, Length(OutputStr));
    DotPos := Pos('.', SubStr);
    if DotPos > 0 then
      MinorVer := StrToIntDef(Copy(SubStr, 1, DotPos - 1), 0)
    else
      MinorVer := StrToIntDef(SubStr, 0);
  end
  else
  begin
    MajorVer := StrToIntDef(OutputStr, 0);
    MinorVer := 1;
  end;

  if (MajorVer >= 13) and (MajorVer <= 16) and (MinorVer >= 1) then
  begin
    writeln('gcc-check: ok (gcc ', OutputStr, ' - stable release)');
    Halt(0);
  end
  else if (MajorVer > 0) and (MajorVer < 13) then
  begin
    writeln(StdErr, 'gcc-check: warning: outdated version (gcc ', OutputStr, ')');
    Halt(1);
  end
  else
  begin
    writeln(StdErr, 'gcc-check: error: unsupported or experimental version (gcc ', OutputStr, ')');
    Halt(1);
  end;
end.
