using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace MML_VisualizersBase
{
  public class LegendItem
  {
    public string? Title { get; set; }
    public Brush? Color { get; set; }
  }

  public partial class LegendWidget : UserControl
  {
    public ObservableCollection<LegendItem> LegendItems
    {
      get { return (ObservableCollection<LegendItem>)GetValue(LegendItemsProperty); }
      set { SetValue(LegendItemsProperty, value); }
    }

    public static readonly DependencyProperty LegendItemsProperty =
        DependencyProperty.Register("LegendItems", typeof(ObservableCollection<LegendItem>), typeof(LegendWidget), new PropertyMetadata(new ObservableCollection<LegendItem>()));

    public LegendWidget()
    {
      InitializeComponent();
    }
  }
}
