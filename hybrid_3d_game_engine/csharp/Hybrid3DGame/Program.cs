using System.Diagnostics;
using System.Text.Json;

var frameCount = args.Length > 0 ? int.Parse(args[0]) : 8;
var interactive = args.Contains("--interactive");
var pythonExe = ResolvePythonExecutable();
var repoRoot = FindRepoRoot(Directory.GetCurrentDirectory());
var playerX = 0.0;
var playerY = 0.0;
var playerZ = 0.0;

for (var frame = 0; frame < frameCount; frame++)
{
    if (interactive)
    {
        if (Console.KeyAvailable)
        {
            var key = Console.ReadKey(true).Key;
            switch (key)
            {
                case ConsoleKey.W:
                case ConsoleKey.UpArrow:
                    playerZ -= 0.2;
                    break;
                case ConsoleKey.S:
                case ConsoleKey.DownArrow:
                    playerZ += 0.2;
                    break;
                case ConsoleKey.A:
                case ConsoleKey.LeftArrow:
                    playerX -= 0.2;
                    break;
                case ConsoleKey.D:
                case ConsoleKey.RightArrow:
                    playerX += 0.2;
                    break;
                case ConsoleKey.Q:
                    playerY += 0.1;
                    break;
                case ConsoleKey.E:
                    playerY -= 0.1;
                    break;
                case ConsoleKey.Escape:
                    Environment.Exit(0);
                    break;
            }
        }
    }
    else
    {
        playerX = Math.Sin(frame * 0.35) * 0.8;
        playerZ = Math.Cos(frame * 0.35) * 0.8;
    }

    var scene = RunPythonFrame(pythonExe, repoRoot, frame, playerX, playerY, playerZ);
    RenderScene(scene);
    Console.WriteLine($"\nFrame {frame} | Python + C# hybrid renderer");
    Console.WriteLine($"Map: {scene.Map?.Name ?? "starter-room"}");
    Console.WriteLine($"Position: ({scene.Player?.X:0.0}, {scene.Player?.Y:0.0}, {scene.Player?.Z:0.0})");
    Console.WriteLine(scene.Hud?.Controls ?? "Controls: WASD/Arrows move, Q/E rotate");

    if (interactive)
    {
        Thread.Sleep(80);
    }
    else
    {
        Thread.Sleep(140);
    }
}

static string ResolvePythonExecutable()
{
    var configured = Environment.GetEnvironmentVariable("PYTHON_EXE");
    if (!string.IsNullOrWhiteSpace(configured))
    {
        return configured;
    }

    foreach (var candidate in new[] { "python", "py" })
    {
        try
        {
            var startInfo = new ProcessStartInfo
            {
                FileName = candidate,
                Arguments = "-V",
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            using var process = Process.Start(startInfo)!;
            process.WaitForExit(3000);
            if (process.ExitCode == 0)
            {
                return candidate;
            }
        }
        catch
        {
            // Ignore and try the next candidate.
        }
    }

    throw new InvalidOperationException("Python executable was not found. Install Python or set PYTHON_EXE.");
}

static SceneState RunPythonFrame(string pythonExe, string repoRoot, int frame, double playerX, double playerY, double playerZ)
{
    var pythonScript = Path.Combine(repoRoot, "python", "game_logic.py");
    var startInfo = new ProcessStartInfo
    {
        FileName = pythonExe,
        Arguments = $"\"{pythonScript}\" --frame {frame} --player-x {playerX} --player-y {playerY} --player-z {playerZ}",
        RedirectStandardOutput = true,
        RedirectStandardError = true,
        UseShellExecute = false,
        CreateNoWindow = true,
        WorkingDirectory = repoRoot
    };

    using var process = Process.Start(startInfo)!;
    var stdout = process.StandardOutput.ReadToEnd();
    var stderr = process.StandardError.ReadToEnd();
    process.WaitForExit();

    if (process.ExitCode != 0)
    {
        throw new InvalidOperationException($"Python failed: {stderr}");
    }

    return JsonSerializer.Deserialize<SceneState>(stdout, new JsonSerializerOptions
    {
        PropertyNameCaseInsensitive = true
    }) ?? new SceneState();
}

static string FindRepoRoot(string startDir)
{
    var current = new DirectoryInfo(startDir);
    while (current != null)
    {
        if (File.Exists(Path.Combine(current.FullName, "python", "game_logic.py")))
        {
            return current.FullName;
        }
        current = current.Parent;
    }

    return startDir;
}

static void RenderScene(SceneState scene)
{
    Console.Clear();
    var width = scene.Screen?.Width ?? 80;
    var height = scene.Screen?.Height ?? 24;
    var buffer = new char[height, width];

    for (var y = 0; y < height; y++)
        for (var x = 0; x < width; x++)
            buffer[y, x] = ' ';

    foreach (var edge in scene.Edges)
    {
        if (edge.Count < 2) continue;
        var start = scene.Vertices[edge[0]];
        var end = scene.Vertices[edge[1]];
        var p1 = ProjectPoint(start, width, height);
        var p2 = ProjectPoint(end, width, height);
        DrawLine(buffer, p1, p2, '#');
    }

    var player = scene.Player;
    if (player != null)
    {
        var playerPoint = ProjectPoint(new Vertex { X = player.X, Y = player.Y, Z = player.Z }, width, height);
        if (playerPoint.X >= 0 && playerPoint.Y >= 0 && playerPoint.X < width && playerPoint.Y < height)
            buffer[playerPoint.Y, playerPoint.X] = '@';
    }

    for (var y = 0; y < height; y++)
    {
        var row = new string(Enumerable.Range(0, width).Select(x => buffer[y, x]).ToArray());
        Console.WriteLine(row);
    }
}

static Point ProjectPoint(Vertex vertex, int width, int height)
{
    const double fov = 600.0;
    const double depth = 4.0;
    var perspective = fov / (depth + vertex.Z);
    var x = (int)Math.Round((vertex.X * perspective) + (width / 2.0));
    var y = (int)Math.Round((-vertex.Y * perspective) + (height / 2.0));
    return new Point(x, y);
}

static void DrawLine(char[,] buffer, Point start, Point end, char ch)
{
    var dx = Math.Abs(end.X - start.X);
    var dy = -Math.Abs(end.Y - start.Y);
    var sx = start.X < end.X ? 1 : -1;
    var sy = start.Y < end.Y ? 1 : -1;
    var error = dx + dy;

    var x = start.X;
    var y = start.Y;

    while (true)
    {
        if (x >= 0 && y >= 0 && x < buffer.GetLength(1) && y < buffer.GetLength(0))
            buffer[y, x] = ch;

        if (x == end.X && y == end.Y) break;
        var error2 = 2 * error;
        if (error2 >= dy)
        {
            error += dy;
            x += sx;
        }
        if (error2 <= dx)
        {
            error += dx;
            y += sy;
        }
    }
}

public class SceneState
{
    public ScreenInfo? Screen { get; set; }
    public List<Vertex> Vertices { get; set; } = new();
    public List<List<int>> Edges { get; set; } = new();
    public PlayerState? Player { get; set; }
    public SceneMap? Map { get; set; }
    public HudInfo? Hud { get; set; }
}

public class ScreenInfo
{
    public int Width { get; set; }
    public int Height { get; set; }
}

public class Vertex
{
    public double X { get; set; }
    public double Y { get; set; }
    public double Z { get; set; }
}

public class PlayerState
{
    public double X { get; set; }
    public double Y { get; set; }
    public double Z { get; set; }
}

public class SceneMap
{
    public string? Name { get; set; }
}

public class HudInfo
{
    public string? Controls { get; set; }
}

public record Point(int X, int Y);
