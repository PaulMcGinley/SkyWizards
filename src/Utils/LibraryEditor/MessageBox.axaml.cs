using System.Threading.Tasks;
using Avalonia.Controls;
using Avalonia.Markup.Xaml;

namespace LibraryEditor;

public partial class MessageBox : Window
{
    public enum MessageBoxButtons
    {
        Ok,
        OkCancel,
        YesNo,
        YesNoCancel
    }

    public enum MessageBoxResult
    {
        Ok,
        Cancel,
        Yes,
        No
    }

    public MessageBox()
    {
        AvaloniaXamlLoader.Load(this);
    }

    public static Task<MessageBoxResult> Show(Window parent, string text, string title, MessageBoxButtons buttons)
    {
        var messageBox = new MessageBox()
        {
            Title = title,
            WindowStartupLocation = WindowStartupLocation.CenterOwner
        };

        messageBox.FindControl<TextBlock>("Text")!.Text = text;
        var buttonPanel = messageBox.FindControl<StackPanel>("Buttons");

        var result = MessageBoxResult.Ok;

        if (buttons is MessageBoxButtons.Ok or MessageBoxButtons.OkCancel)
        {
            AddButton("Ok", MessageBoxResult.Ok, true);
        }

        if (buttons is MessageBoxButtons.YesNo or MessageBoxButtons.YesNoCancel)
        {
            AddButton("Yes", MessageBoxResult.Yes);
            AddButton("No", MessageBoxResult.No, true);
        }

        if (buttons is MessageBoxButtons.OkCancel or MessageBoxButtons.YesNoCancel)
            AddButton("Cancel", MessageBoxResult.Cancel, true);

        var tcs = new TaskCompletionSource<MessageBoxResult>();
        messageBox.Closed += delegate { tcs.TrySetResult(result); };

        if (parent != null)
        {
            messageBox.ShowDialog(parent);
        }
        else
        {
            messageBox.Show();
        }

        // Return the task that completes when the message box is closed
        return tcs.Task;
        
        void AddButton(string caption, MessageBoxResult r, bool def = false)
        {
            var btn = new Button { Content = caption };
            btn.Click += (_, __) =>
            {
                result = r;
                messageBox.Close();
            };
            buttonPanel?.Children.Add(btn);
            if (def)
                result = r;
        }
    }
}