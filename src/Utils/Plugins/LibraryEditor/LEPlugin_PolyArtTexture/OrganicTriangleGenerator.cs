using SkiaSharp;

namespace LEPlugin_PolyArtTexture;

public class OrganicTriangleGenerator
{
    private readonly int width;
    private readonly int height;
    private readonly int baseTriangleSize;
    private readonly Random random;
    private readonly HashSet<Edge> usedEdges;
    private readonly List<Triangle> triangles;
    private readonly float randomnessRange;

    public struct Edge
    {
        public SKPoint V1 { get; }
        public SKPoint V2 { get; }

        public Edge(SKPoint v1, SKPoint v2)
        {
            // Always store edge points in consistent order for comparison
            if (v1.X < v2.X || (v1.X == v2.X && v1.Y < v2.Y))
            {
                V1 = v1;
                V2 = v2;
            }
            else
            {
                V1 = v2;
                V2 = v1;
            }
        }

        public override bool Equals(object obj)
        {
            if (obj is Edge other)
            {
                return Math.Abs(V1.X - other.V1.X) < 0.1f &&
                       Math.Abs(V1.Y - other.V1.Y) < 0.1f &&
                       Math.Abs(V2.X - other.V2.X) < 0.1f &&
                       Math.Abs(V2.Y - other.V2.Y) < 0.1f;
            }
            return false;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(V1.X, V1.Y, V2.X, V2.Y);
        }
    }

    public class Triangle
    {
        public SKPoint V1 { get; }
        public SKPoint V2 { get; }
        public SKPoint V3 { get; }
        public SKColor Color { get; }

        public Triangle(SKPoint v1, SKPoint v2, SKPoint v3, SKColor color)
        {
            V1 = v1;
            V2 = v2;
            V3 = v3;
            Color = color;
        }

        public IEnumerable<Edge> GetEdges()
        {
            yield return new Edge(V1, V2);
            yield return new Edge(V2, V3);
            yield return new Edge(V3, V1);
        }
    }

    public OrganicTriangleGenerator(int width, int height, int baseTriangleSize, float randomnessRange = 0.5f)
    {
        this.width = width;
        this.height = height;
        this.baseTriangleSize = baseTriangleSize;
        this.randomnessRange = randomnessRange;
        this.random = new Random();
        this.usedEdges = new HashSet<Edge>();
        this.triangles = new List<Triangle>();
    }

    private SKPoint GenerateThirdVertex(SKPoint v1, SKPoint v2, bool flipDirection)
    {
        // Calculate the midpoint
        float midX = (v1.X + v2.X) / 2;
        float midY = (v1.Y + v2.Y) / 2;

        // Calculate the perpendicular vector
        float edgeX = v2.X - v1.X;
        float edgeY = v2.Y - v1.Y;
        float perpX = -edgeY;
        float perpY = edgeX;

        // Normalize the perpendicular vector
        float length = (float)Math.Sqrt(perpX * perpX + perpY * perpY);
        perpX /= length;
        perpY /= length;

        // Calculate the height with randomness
        float height = baseTriangleSize * (float)(0.8 + random.NextDouble() * 0.4);
        
        // Flip direction if needed
        if (flipDirection)
        {
            perpX = -perpX;
            perpY = -perpY;
        }

        // Add some random variation to the perpendicular direction
        float angleVariation = (float)(random.NextDouble() - 0.5) * randomnessRange;
        float cos = (float)Math.Cos(angleVariation);
        float sin = (float)Math.Sin(angleVariation);
        float finalPerpX = perpX * cos - perpY * sin;
        float finalPerpY = perpX * sin + perpY * cos;

        return new SKPoint(
            midX + finalPerpX * height,
            midY + finalPerpY * height
        );
    }

    public byte[] GeneratePattern()
    {
        using (var surface = SKSurface.Create(new SKImageInfo(width, height)))
        {
            var canvas = surface.Canvas;
            canvas.Clear(SKColors.White);

            var baseColors = new[]
            {
                new SKColor(255, 165, 0),    // Orange
                new SKColor(255, 140, 0),    // Dark Orange
                new SKColor(255, 190, 0)     // Light Orange
            };

            // Create initial triangle in the center
            var centerX = width / 2f;
            var centerY = height / 2f;
            var v1 = new SKPoint(centerX - baseTriangleSize, centerY);
            var v2 = new SKPoint(centerX + baseTriangleSize, centerY);
            var v3 = GenerateThirdVertex(v1, v2, false);

            TryAddTriangle(new Triangle(v1, v2, v3, VaryColor(baseColors[0])));

            // Queue for processing edges
            Queue<Edge> edgesToProcess = new Queue<Edge>();
            foreach (var edge in triangles[0].GetEdges())
            {
                edgesToProcess.Enqueue(edge);
            }

            // Process edges and grow triangles
            while (edgesToProcess.Count > 0 && triangles.Count < 1000)
            {
                var edge = edgesToProcess.Dequeue();
                
                // Try both directions for the third vertex
                foreach (bool flipDirection in new[] { true, false })
                {
                    var newV3 = GenerateThirdVertex(edge.V1, edge.V2, flipDirection);

                    // Check if the new vertex is within bounds
                    if (newV3.X < 0 || newV3.X > width || newV3.Y < 0 || newV3.Y > height)
                        continue;

                    var newTriangle = new Triangle(edge.V1, edge.V2, newV3, 
                        VaryColor(baseColors[random.Next(baseColors.Length)]));

                    if (TryAddTriangle(newTriangle))
                    {
                        // Add new edges to process
                        foreach (var newEdge in newTriangle.GetEdges())
                        {
                            if (!usedEdges.Contains(newEdge))
                            {
                                edgesToProcess.Enqueue(newEdge);
                            }
                        }
                        break;  // Successfully added triangle, move to next edge
                    }
                }
            }

            // Draw all triangles
            foreach (var triangle in triangles)
            {
                DrawTriangle(canvas, triangle);
            }

            // Save the image to memory stream
            using (var image = surface.Snapshot())
            using (var data = image.Encode(SKEncodedImageFormat.Png, 100))
            using (var stream = new MemoryStream())
            {
                data.SaveTo(stream);
                return stream.ToArray();
            }
        }
    }

    private bool TryAddTriangle(Triangle triangle)
    {
        // Check if any edges are already used
        foreach (var edge in triangle.GetEdges())
        {
            if (usedEdges.Contains(edge))
                return false;
        }

        // Add the triangle and mark its edges as used
        triangles.Add(triangle);
        foreach (var edge in triangle.GetEdges())
        {
            usedEdges.Add(edge);
        }

        return true;
    }

    private void DrawTriangle(SKCanvas canvas, Triangle triangle)
    {
        using (var path = new SKPath())
        using (var paint = new SKPaint())
        {
            path.MoveTo(triangle.V1);
            path.LineTo(triangle.V2);
            path.LineTo(triangle.V3);
            path.Close();

            paint.IsAntialias = true;
            paint.Style = SKPaintStyle.Fill;
            paint.Color = triangle.Color;

            canvas.DrawPath(path, paint);
        }
    }

    private SKColor VaryColor(SKColor baseColor)
    {
        int variation = 15;
        return new SKColor(
            (byte)Math.Clamp(baseColor.Red + random.Next(-variation, variation), 0, 255),
            (byte)Math.Clamp(baseColor.Green + random.Next(-variation, variation), 0, 255),
            (byte)Math.Clamp(baseColor.Blue + random.Next(-variation, variation), 0, 255)
        );
    }
}