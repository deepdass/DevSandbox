import re
import sys
import shutil
import argparse
from pathlib import Path
from dataclasses import dataclass, field
from typing import List, Tuple, Optional

DEFAULT_EXTENSIONS = (".h", ".cpp")

UPROPERTY_LINE = re.compile(r'^(\s*)UPROPERTY\s*\(')
PTR_LINE = re.compile(
    r'^(\s*)(const\s+)?([UA][A-Za-z0-9_]+)\s*\*\s*(const\s*)?([A-Za-z_][A-Za-z0-9_]*)([^;]*;)\s*$'
)

@dataclass
class Change:
    line_number: int
    original: str
    replaced: str

@dataclass
class FileResult:
    path: Path
    changes: List[Change] = field(default_factory=list)
    error: Optional[str] = None

    @property
    def changed(self) -> bool:
        return len(self.changes) > 0


def transform_text(source: str) -> Tuple[str, List[Tuple[int, str, str]]]:
    changes: List[Tuple[int, str, str]] = []
    lines = source.splitlines(keepends=True)
    out = []
    i = 0
    while i < len(lines):
        line = lines[i]
        if UPROPERTY_LINE.match(line) and "TObjectPtr" not in line:
            uproperty_line = line.rstrip("\r\n")
            if i + 1 < len(lines):
                next_line = lines[i + 1]
                m = PTR_LINE.match(next_line.rstrip("\r\n"))
                if m and "TObjectPtr" not in next_line:
                    indent         = m.group(1)
                    leading_const  = m.group(2) or ""
                    type_name      = m.group(3)
                    trailing_const = m.group(4) or ""
                    var_name       = m.group(5)
                    rest           = m.group(6)

                    if leading_const.strip():
                        wrapped = f"TObjectPtr<const {type_name}>"
                    else:
                        wrapped = f"TObjectPtr<{type_name}>"
                    if trailing_const.strip():
                        wrapped = f"const {wrapped}"

                    ending = "\r\n" if next_line.endswith("\r\n") else "\n"
                    new_next = f"{indent}{wrapped} {var_name}{rest}{ending}"

                    original = uproperty_line + "\n" + next_line.rstrip("\r\n")
                    replaced = uproperty_line + "\n" + new_next.rstrip("\r\n")
                    changes.append((i + 1, original, replaced))

                    out.append(line)
                    out.append(new_next)
                    i += 2
                    continue
        out.append(line)
        i += 1

    return "".join(out), changes


def process_file(path: Path, dry_run: bool, backup: bool) -> FileResult:
    result = FileResult(path=path)

    try:
        source = path.read_text(encoding="utf-8", errors="replace")
    except Exception as e:
        result.error = str(e)
        return result

    new_text, raw_changes = transform_text(source)

    for line_num, original, replaced in raw_changes:
        result.changes.append(Change(
            line_number=line_num,
            original=original.strip(),
            replaced=replaced.strip(),
        ))

    if result.changed and not dry_run:
        if backup:
            shutil.copy2(path, path.with_suffix(path.suffix + ".bak"))
        try:
            path.write_text(new_text, encoding="utf-8")
        except Exception as e:
            result.error = str(e)

    return result


def collect_files(root: Path, extensions: Tuple[str, ...]) -> List[Path]:
    skip_dirs = {".git", "Binaries", "Intermediate", "Saved", "DerivedDataCache", "node_modules"}
    files = []
    for p in root.rglob("*"):
        if any(part in skip_dirs for part in p.parts):
            continue
        if p.suffix in extensions:
            files.append(p)
    return sorted(files)


RESET  = "\033[0m"
RED    = "\033[31m"
GREEN  = "\033[32m"
YELLOW = "\033[33m"
CYAN   = "\033[36m"
BOLD   = "\033[1m"
DIM    = "\033[2m"

def use_color() -> bool:
    return sys.stdout.isatty()

def c(code: str, text: str) -> str:
    return f"{code}{text}{RESET}" if use_color() else text


def print_result(result: FileResult, verbose: bool) -> None:
    if result.error:
        print(c(RED, f"  ERROR  ") + str(result.path) + f"  →  {result.error}")
        return

    if not result.changed:
        if verbose:
            print(c(DIM, f"  skip   ") + str(result.path))
        return

    print(c(GREEN, f"  changed") + f" {result.path}  ({len(result.changes)} replacement{'s' if len(result.changes)!=1 else ''})")
    for ch in result.changes:
        print(c(DIM, f"    L{ch.line_number:<5}"))
        print(c(RED,   f"    - {ch.original}"))
        print(c(GREEN, f"    + {ch.replaced}"))
        print()


def print_summary(results: List[FileResult], dry_run: bool) -> None:
    total_files   = len(results)
    changed_files = sum(1 for r in results if r.changed)
    total_changes = sum(len(r.changes) for r in results)
    errors        = sum(1 for r in results if r.error)

    mode = c(YELLOW, " [DRY RUN — no files written]") if dry_run else ""
    print()
    print(c(BOLD, "=" * 60))
    print(c(BOLD, "Summary") + mode)
    print(c(BOLD, "=" * 60))
    print(f"  Files scanned : {total_files}")
    print(f"  Files changed : {c(GREEN, str(changed_files))}")
    print(f"  Replacements  : {c(GREEN, str(total_changes))}")
    if errors:
        print(f"  Errors        : {c(RED, str(errors))}")
    print()


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert UE4 raw UPROPERTY pointers to UE5 TObjectPtr<T>."
    )
    parser.add_argument("root", type=Path)
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--backup", action="store_true")
    parser.add_argument("--ext", nargs="+", default=list(DEFAULT_EXTENSIONS), metavar="EXT")
    parser.add_argument("--verbose", action="store_true")
    parser.add_argument("--file", type=Path, default=None, metavar="FILE")
    args = parser.parse_args()

    extensions = tuple(e if e.startswith(".") else f".{e}" for e in args.ext)

    if args.file:
        files = [args.file]
    else:
        if not args.root.is_dir():
            print(c(RED, f"Error: '{args.root}' is not a directory."))
            sys.exit(1)
        print(f"Scanning {c(CYAN, str(args.root))} for {extensions} files …")
        files = collect_files(args.root, extensions)
        print(f"Found {len(files)} file(s).\n")

    results: List[FileResult] = []
    for path in files:
        result = process_file(path, dry_run=args.dry_run, backup=args.backup)
        results.append(result)
        print_result(result, verbose=args.verbose)

    print_summary(results, dry_run=args.dry_run)

    if args.dry_run:
        print(c(YELLOW, "  Re-run without --dry-run to apply changes.\n"))


if __name__ == "__main__":
    main()
