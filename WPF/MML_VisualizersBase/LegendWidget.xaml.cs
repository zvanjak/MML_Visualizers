using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;

namespace MML_VisualizersBase
{
  /// <summary>
  /// Converts a boolean to opacity (1.0 for true, 0.4 for false).
  /// </summary>
  public class BoolToOpacityConverter : IValueConverter
  {
    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
      if (value is bool isVisible)
        return isVisible ? 1.0 : 0.4;
      return 1.0;
    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
      throw new NotImplementedException();
    }
  }

  /// <summary>
  /// Represents a single item in the legend with visibility toggle support.
  /// </summary>
  public class LegendItem : INotifyPropertyChanged
  {
    private string? _title;
    private Brush? _color;
    private bool _isVisible = true;

    public string? Title
    {
      get => _title;
      set { _title = value; OnPropertyChanged(); }
    }

    public Brush? Color
    {
      get => _color;
      set { _color = value; OnPropertyChanged(); }
    }

    public bool IsVisible
    {
      get => _isVisible;
      set
      {
        if (_isVisible != value)
        {
          _isVisible = value;
          OnPropertyChanged();
          VisibilityChanged?.Invoke(this, EventArgs.Empty);
        }
      }
    }

    /// <summary>
    /// Event raised when the visibility of this item changes.
    /// </summary>
    public event EventHandler? VisibilityChanged;

    public event PropertyChangedEventHandler? PropertyChanged;

    protected virtual void OnPropertyChanged([CallerMemberName] string? propertyName = null)
    {
      PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
  }

  /// <summary>
  /// A legend widget that displays a list of items with color indicators and visibility checkboxes.
  /// </summary>
  public partial class LegendWidget : UserControl
  {
    public ObservableCollection<LegendItem> LegendItems
    {
      get { return (ObservableCollection<LegendItem>)GetValue(LegendItemsProperty); }
      set { SetValue(LegendItemsProperty, value); }
    }

    public static readonly DependencyProperty LegendItemsProperty =
        DependencyProperty.Register("LegendItems", typeof(ObservableCollection<LegendItem>), typeof(LegendWidget),
          new PropertyMetadata(new ObservableCollection<LegendItem>()));

    /// <summary>
    /// Event raised when any legend item's visibility changes.
    /// </summary>
    public event EventHandler? ItemVisibilityChanged;

    public LegendWidget()
    {
      InitializeComponent();
      LegendItems = new ObservableCollection<LegendItem>();
      LegendItems.CollectionChanged += LegendItems_CollectionChanged;
    }

    private void LegendItems_CollectionChanged(object? sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
    {
      // Subscribe to visibility changes for new items
      if (e.NewItems != null)
      {
        foreach (LegendItem item in e.NewItems)
        {
          item.VisibilityChanged += Item_VisibilityChanged;
        }
      }

      // Unsubscribe from old items
      if (e.OldItems != null)
      {
        foreach (LegendItem item in e.OldItems)
        {
          item.VisibilityChanged -= Item_VisibilityChanged;
        }
      }
    }

    private void Item_VisibilityChanged(object? sender, EventArgs e)
    {
      ItemVisibilityChanged?.Invoke(sender, e);
    }
  }
}
