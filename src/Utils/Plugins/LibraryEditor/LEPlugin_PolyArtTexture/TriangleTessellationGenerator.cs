using Avalonia;
using Avalonia.Media;
using Avalonia.Media.Imaging;

namespace LEPlugin_PolyArtTexture
{
    public class TriangleTessellationGenerator
    {
        private static Random random = new Random();

        public static libType.LImage GeneratePolyArt(int width, int height, int gridSize)
        {
            var points = GenerateGridPoints(width, height, gridSize);
            ApplyRandomOffset(points, gridSize / 24);
            var triangles = Triangulate(points);
            var bitmap = DrawTriangles(triangles, width, height);
    
            using (var stream = new MemoryStream())
            {
                bitmap.Save(stream);
                var lImage = new libType.LImage
                {
                    Data = stream.ToArray()
                };
                return lImage;
            }
        }

        private static List<Point> GenerateGridPoints(int width, int height, int gridSize)
        {
            var points = new List<Point>();
            for (int y = 0; y <= height; y += gridSize)
            {
                for (int x = 0; x <= width; x += gridSize)
                {
                    points.Add(new Point(x, y));
                }
            }

            return points;
        }

        private static void ApplyRandomOffset(List<Point> points, int maxOffset)
        {
            for (int i = 0; i < points.Count; i++)
            {
                var offsetX = random.Next(-maxOffset, maxOffset);
                var offsetY = random.Next(-maxOffset, maxOffset);
                points[i] = new Point(points[i].X + offsetX, points[i].Y + offsetY);
            }
        }

        private static List<Tuple<Point, Point, Point>> Triangulate(List<Point> points)
        {
            var triangles = new List<Tuple<Point, Point, Point>>();
            int gridSize = (int)Math.Sqrt(points.Count);
            for (int y = 0; y < gridSize - 1; y++)
            {
                for (int x = 0; x < gridSize - 1; x++)
                {
                    int index = y * gridSize + x;
                    var p1 = points[index];
                    var p2 = points[index + 1];
                    var p3 = points[index + gridSize];
                    var p4 = points[index + gridSize + 1];

                    triangles.Add(Tuple.Create(p1, p2, p3));
                    triangles.Add(Tuple.Create(p2, p3, p4));
                }
            }

            return triangles;
        }

        private static RenderTargetBitmap DrawTriangles(List<Tuple<Point, Point, Point>> triangles, int width,
            int height)
        {
            var bitmap = new RenderTargetBitmap(new PixelSize(width, height), new Vector(96, 96));
            using (var ctx = bitmap.CreateDrawingContext())
            {
                foreach (var triangle in triangles)
                {
                    var brush = new SolidColorBrush(Color.FromArgb(255, (byte)random.Next(256), (byte)random.Next(256),
                        (byte)random.Next(256)));
                    var points = new[] { triangle.Item1, triangle.Item2, triangle.Item3 };
                    var poly = new PolylineGeometry(points, true);
                    ctx.DrawGeometry(brush, null, poly);
                }
            }

            return bitmap;
        }
    }
}