
using IotDemo.ViewModels;

using Xamarin.Forms;

namespace IotDemo.Views
{
	public partial class MedicineInfoPage : ContentPage
	{
		ItemDetailViewModel viewModel;

        // Note - The Xamarin.Forms Previewer requires a default, parameterless constructor to render a page.
        public MedicineInfoPage()
        {
            InitializeComponent();
        }

        public MedicineInfoPage(ItemDetailViewModel viewModel)
		{
			InitializeComponent();

			BindingContext = this.viewModel = viewModel;
		}
	}
}
