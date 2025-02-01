using SkiaSharp;

namespace LEPlugin_PolyArtTexture_AB
{
    public struct Node
    {
        public int Id { get; set; }
        public double X { get; set; }
        public double Y { get; set; }
    }

    public struct NodeWithNeighbors
    {
        public Node Node { get; set; }
        public List<Node> NearestNeighbors { get; set; }
    }

    public class RandomNodeGenerator
    {
        private readonly int density;
        private readonly Random random;

        public RandomNodeGenerator(int density)
        {
            this.density = density;
            this.random = new Random();
        }

        public List<NodeWithNeighbors> GenerateNodes(int width, int height)
        {
            var nodes = new List<Node>();
            for (int i = 0; i < density; i++)
            {
                nodes.Add(new Node
                {
                    Id = i,
                    X = random.NextDouble() * width,
                    Y = random.NextDouble() * height
                });
            }

            var nodesWithNeighbors = new List<NodeWithNeighbors>();
            foreach (var node in nodes)
            {
                var nearestNeighbors = nodes
                    .Where(n => n.Id != node.Id)
                    .OrderBy(n => GetDistance(node, n))
                    .Take(8)
                    .ToList();

                nodesWithNeighbors.Add(new NodeWithNeighbors
                {
                    Node = node,
                    NearestNeighbors = nearestNeighbors
                });
            }

            return nodesWithNeighbors;
        }

        private double GetDistance(Node a, Node b)
        {
            return Math.Sqrt(Math.Pow(a.X - b.X, 2) + Math.Pow(a.Y - b.Y, 2));
        }

        public byte[] RenderTexture(List<NodeWithNeighbors> nodesWithNeighbors, int width, int height)
        {
            using (var bitmap = new SKBitmap(width, height))
            using (var canvas = new SKCanvas(bitmap))
            {
                 canvas.Clear(SKColors.Orange);

                // Define colors close to yellow
                var colors = new[]
                {
                    new SKColor(255, 190, 140),    // Yellow
                    new SKColor(255, 165, 0),  // Light Yellow
                    new SKColor(200, 120, 0)   // Pale Yellow
                };

                foreach (var nodeWithNeighbors in nodesWithNeighbors)
                {
                    var node = nodeWithNeighbors.Node;
                    var neighbors = nodeWithNeighbors.NearestNeighbors;

                    if (neighbors.Count < 3)
                        continue;

                    for (int i = 0; i < neighbors.Count - 2; i++)
                    {
                        var p1 = new SKPoint((float)node.X, (float)node.Y);
                        var p2 = new SKPoint((float)neighbors[i].X, (float)neighbors[i].Y);
                        var p3 = new SKPoint((float)neighbors[i + 1].X, (float)neighbors[i + 1].Y);

                        // Draw lines from each node to its nearest neighbors
                        using (var paint = new SKPaint())
                        {
                            paint.Color = SKColors.Black;
                            paint.StrokeWidth = 1;
                            paint.IsAntialias = true;
                            canvas.DrawLine(p1, p2, paint);
                            canvas.DrawLine(p2, p3, paint);
                            canvas.DrawLine(p3, p1, paint);
                        }

                        // Color the area of each created triangle
                        using (var paint = new SKPaint())
                        {
                            paint.Color = colors[random.Next(colors.Length)];
                            paint.IsAntialias = true;
                            var path = new SKPath();
                            path.MoveTo(p1);
                            path.LineTo(p2);
                            path.LineTo(p3);
                            path.Close();
                            canvas.DrawPath(path, paint);
                        }
                    }
                }

                using (var image = SKImage.FromBitmap(bitmap))
                using (var data = image.Encode(SKEncodedImageFormat.Png, 100))
                {
                    return data.ToArray();
                }
            }
        }
    }
}
