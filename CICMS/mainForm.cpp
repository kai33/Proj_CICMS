/*************************************************************************************************/
//
//  class mainForm: mainForm.cpp
//
//  Description: mainForm.h contains the implementation part of class mainForm.
//
//  API:
//  CICMS_UI::mainForm::mainForm(); //create a mainForm object
//
//  Main authors: XIE KAI(A0102016E), 
//
/*************************************************************************************************/
//
//  Naming Rule for variables
//
//  for variables, we name them as follow:
//  [abbreviation for grp]_[abbreviation for type of components/its parent in menu]_[name of component]
//  E.g.
//  "list_b_delete" means a button for deletion in list group
//  "list_l_name" means a label for name in list group
////////////////////////////////////
//
//  options for [abbreviation for grp] (grp, or group/groupBox, is where the components locate)
//
//  menu;
//  list;
//  s, for search;
//  other: whatever :)
////////////////////////////////////
//
//  options for [abbreviation for type of components/its parent in menu]
//
//  f, for file;
//  b, for button;
//  tB, for textBox;
//  lv, for listview;
//  selectedList;
//  grp, for groupBox;
//  l, for label;
//  rB, for radioBox;
//  col, for column;
//  other: whatever :)
//  But if that component is in menu and it has no parent, then this part is no need
////////////////////////////////////
//
//  options for [name of component]
//
//  you decide :) be meaningful
//
/*************************************************************************************************/
//
//  Naming Rule for functions
//
//  for functions, we name them as follows:
//  [Verb]_[abbreviation for grp]_[abbreviation for kind of components/its parent in menu]_[name of component]_[Objective]
//  or
//  [Verb]_[name of component/name of class]_[Objective]
//  or
//  whatever you like :) be meaningful
//  E.g.
//  "Update_selectedList_Sell" means update all selected listItems's sold property.
//
/*************************************************************************************************/
//
//  Naming Rule for events
//
//  for event, we name them as follows:
//  [abbreviation for grp]_[abbreviation for kind of components/its parent in menu]_[name of component]_[Verb]
//  or
//  [name of component/name of class]_[Verb]
//  or
//  whatever you like :) be meaningful
//  E.g.
//  "menu_about_Click" means the event - when user clicks the about item in the menu.
//
/*************************************************************************************************/

#include "stdafx.h"
#include "mainForm.h"
#include "inputForm.h"
#include "addPdForm.h"
#include "ListViewItemComparer.h"
#include "InputCheck.h"
#include <cliext/vector>
//#include <vector>
//#include <string>

using namespace CICMS_UI;

//definitions for Update_statusBar()'s parameter
enum STATUSBAR {
	addS, addF,
	loadS, loadF,
	saveS, saveF,
	sellS, sellF,
	restockS, restockF,
	deleteS, deleteF,
	discountS, discountF,
	searchS, searchF
};

enum BYMETHOD { byName, byBarcode, byCategory, byManuf, byStock };

enum SORT_ORDER{Descending = false, Ascending = true};

//********************************************************
//*************                            ***************
//*************MEMBER FUNCTION DECLEARATION***************
//*************                            ***************
//********************************************************

//*********************************************
//**********MENU COMPONENTS FUNCTION***********
//*********************************************

//Event: when click menu_f_quit item, close the mainForm window
void mainForm::menu_f_quit_Click(System::Object^  sender, System::EventArgs^  e) {
	this->Close();
}
//Event: when click menu_f_addNewProducts item, open the addPdForm window to add new products
void mainForm::menu_f_addNewProducts_Click(System::Object^  sender, System::EventArgs^  e) {
	this->Create_addPdForms();
}
//Event: when click menu_stat_BSpd_Click item, open the MessageBox window to show the result of Best-Selling product(s)
void mainForm::menu_stat_BSpd_Click(System::Object^  sender, System::EventArgs^  e){
	System::String^ output = Bridging->Gen_BSpd();
	if(output->Length != 0)
		System::Windows::Forms::MessageBox::Show("The Best-Selling product(s): \n" + output);
	else
		System::Windows::Forms::MessageBox::Show("Report not available.");
}
//Event: when click menu_stat_BSmanu_Click item, open the MessageBox window to show the result of Best-Selling manufacturer(s)
void mainForm::menu_stat_BSmanu_Click(System::Object^  sender, System::EventArgs^  e){
	System::String^ output = Bridging->Gen_BSmanu();
	if(output->Length != 0)
		System::Windows::Forms::MessageBox::Show("The Best-Selling manufacturer(s): \n" + output);
	else
		System::Windows::Forms::MessageBox::Show("Report not available.");
}
//Event: when click menu_stat_topXpd_Click item, open the inputForm to take in a number X, then open MessageBox window to show the result of Top X Selling products
void mainForm::menu_stat_topXpd_Click(System::Object^  sender, System::EventArgs^  e){
	int x = (int) this->Create_inputForm(" The Top X Selling products", "Please input a number for X.", "X is equal to", "5");
	if(x == 0)//if cancel the MessageBox
		return;
	System::String^ output = Bridging->Gen_TopXpd(x);
	if(output->Length != 0)
		System::Windows::Forms::MessageBox::Show("The Top " + x.ToString() + " Selling products: \n" + output);
	else
		System::Windows::Forms::MessageBox::Show("Report not available.");
}
//Event: when click menu_about item, open a messageBox that contains our team's description
void mainForm::menu_about_Click(System::Object^  sender, System::EventArgs^  e) {
	System::Windows::Forms::MessageBox::Show("Hello! Our team: Ashray, Bob, Hui and Kai!", " About");
}
//Fuction: create a addPdForm window, and let logic/handler part handle the input
void mainForm::Create_addPdForms(){
	int num = (int) Create_inputForm(" Add products", "How many products to add?", "Number:", "1");
	for( int i = 0; i < num; i++){
		addPdForm^ dlg = gcnew addPdForm();
		dlg->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
		System::Windows::Forms::DialogResult r = dlg->ShowDialog();
		if (r == System::Windows::Forms::DialogResult::OK){
			if(Bridging->Add(dlg->get_product_details()))
				this->Update_statusBar(addS);
			else
				this->Update_statusBar(addF);
		}
		else if(r == System::Windows::Forms::DialogResult::Cancel)
			break;
	}
}

//***********************************************
//**********SEARCH COMPONENTS FUNCTION***********
//***********************************************

//Event: when Text in s_tB_input is changed, used for Search Instant
void mainForm::s_tB_input_TextChanged(System::Object^  sender, System::EventArgs^  e){
	if(this->s_tB_input->Text->Length > 0 && this->s_tB_input->Text != this->last_keyword){
		this->last_keyword = this->s_tB_input->Text;
		this->s_b_submit->PerformClick();
	}
}
//Event: when s_tB_input is clicked
void mainForm::s_tB_input_Click(System::Object^  sender, System::EventArgs^  e){
	if(this->SelectAll_toggle == true){
		this->s_tB_input->SelectAll();
		this->SelectAll_toggle = false;
	}
}
//Event: when s_tB_input is lost focus
void mainForm::s_tB_input_LostFocus(System::Object^  sender, System::EventArgs^  e){
	this->SelectAll_toggle = true;
}
//Event: when s_rB_byName is selected
void mainForm::s_rB_byName_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
	if(this->Get_byMethod() == byName){
		this->s_tB_input->Focus();
		this->s_tB_input->SelectAll();
		this->SelectAll_toggle = false;
	}
}
//Event: when s_rB_byCategory is selected
void mainForm::s_rB_byCategory_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
	if(this->Get_byMethod() == byCategory){
		this->s_tB_input->Focus();
		this->s_tB_input->SelectAll();
		this->SelectAll_toggle = false;
	}
}
//Event: when s_rB_byBarcode is selected
void mainForm::s_rB_byBarcode_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
	if(this->Get_byMethod() == byBarcode){
		if(this->s_tB_input->Text->Length > 9)
			this->s_tB_input->Text = this->s_tB_input->Text->Substring(0,9);
		this->s_tB_input->MaxLength = 9;
		this->s_tB_input->Focus();
		this->s_tB_input->SelectAll();
		this->SelectAll_toggle = false;
	}
	else{
		this->s_tB_input->MaxLength = 21;
	}
}
//Event: when s_rB_byManufacturer is selected
void mainForm::s_rB_byManufacturer_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
	if(this->Get_byMethod() == byManuf){
		this->s_tB_input->Focus();
		this->s_tB_input->SelectAll();
		this->SelectAll_toggle = false;
	}
}
//Event: when s_b_submit button is clicked
void mainForm::s_b_submit_Click(System::Object^  sender, System::EventArgs^  e){
	if(InputCheck::is_empty(this->s_tB_input->Text))
		System::Windows::Forms::MessageBox::Show("Please fill in the search field.");
	else if(this->Get_byMethod() == byBarcode && !InputCheck::is_int(this->s_tB_input->Text))
		;//System::Windows::Forms::MessageBox::Show("Please input an integer in the search field.");
	else if(this->Get_byMethod() == byBarcode && InputCheck::lessThan_zero(this->s_tB_input->Text))
		;//System::Windows::Forms::MessageBox::Show("Please input an integer larger than zero in the search field.");
	else
		this->Search_product(this->s_tB_input->Text, this->Get_byMethod());
}
//Function: get the result of checked radioButton (by which method to search)
int mainForm::Get_byMethod(){
	if(this->s_rB_byName->Checked == true)
		return byName;
	else if(this->s_rB_byBarcode->Checked == true)
		return byBarcode;
	else if(this->s_rB_byCategory->Checked == true)
		return byCategory;
	else if(this->s_rB_byManufacturer->Checked == true)
		return byManuf;
	else
		return byName;
}
//Function: search the products according to a text and a method; if the result is non-empty, it will add an item onto the listView component
void mainForm::Search_product(System::String^ s, int m){
	cliext::vector<System::Windows::Forms::ListViewItem^>^ r = Bridging->Search(s, m);
	if(!r->empty()){
		this->list_lv->BeginUpdate();
		this->list_lv->Items->Clear();
		this->list_lv->Items->AddRange(r->to_array());
		this->list_lv->EndUpdate();
		this->list_lv->Items[0]->Selected = true;
		this->Update_statusBar(searchS);
	}
	else
		this->Update_statusBar(searchF);
}
//*****************************************************
//**********LIST DETAILS COMPONENTS FUNCTION***********
//*****************************************************

//Event: when click pd_b_sell button, open an inputForm window for input sale data
void mainForm::list_b_sell_Click(System::Object^  sender, System::EventArgs^  e) {
	Create_sellForm();
}
//Event: when click pd_b_restock button, open an inputForm window for input restock data
void mainForm::list_b_restock_Click(System::Object^  sender, System::EventArgs^  e) {
	Create_restockForm();
}

//Event: when click pd_b_delete button, open a msgBox to check whether delete the selectedItem or not
void mainForm::list_b_delete_Click(System::Object^  sender, System::EventArgs^  e) {
	Create_deleteForm();
}
//Function: create an inputForm for input; used by pd_b_sell_Click & pd_b_restock_Click events.
double mainForm::Create_inputForm(System::String^ formTitle, System::String^ pdDescript, System::String^ inputDescript, System::String^ stringInTB){
	inputForm^ dlg = gcnew inputForm();
	dlg->set_inputForm(formTitle, pdDescript, inputDescript, stringInTB);
	dlg->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
	if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		return dlg->get_input();
	else
		return 0;// 0 is unique, as user cannot input zero
}
void mainForm::Create_sellForm(){
	for(int i = 0; i < this->list_lv->SelectedItems->Count; i++){
		unsigned num = (unsigned) this->Create_inputForm(" Sell a product", this->Get_sName(i) + " - " + this->Get_sBarcode(i), "Sell:", "1");
		if(num == 0)//if cancel the MessageBox
			break;
		else if(Bridging->Sell(this->list_lv->SelectedItems[i], num)){
			this->Update_selectedItem_sell(i, num);
			this->Update_statusBar(sellS);
		}
		else
			this->Update_statusBar(sellF);
	}
}
void mainForm::Create_restockForm(){
	for(int i = 0; i < this->list_lv->SelectedItems->Count; i++){
		unsigned num = (unsigned) this->Create_inputForm(" Restock a product", this->Get_sName(i) + " - " + this->Get_sBarcode(i), "Restock:", "1");
		if(num == 0)//if cancel the MessageBox
			break;
		else if(Bridging->Restock(this->list_lv->SelectedItems[i], num)){
			this->Update_selectedItem_restock(i, num);
			this->Update_statusBar(restockS);
		}
		else
			this->Update_statusBar(restockF);
	}
}

void mainForm::Create_deleteForm(){
	enum { No_No, Yes_Yes, tooMany = 1};
	int case_tooMany = No_No;
	System::Windows::Forms::DialogResult r;
	if(this->list_lv->SelectedItems->Count > tooMany){
		case_tooMany = Yes_Yes;
		if(this->list_lv->SelectedItems->Count > 2)
			r = (System::Windows::Forms::MessageBox::Show("Are you sure that you would like \nto delete all the selected products?", " Delete Product",
		System::Windows::Forms::MessageBoxButtons::YesNo,
		System::Windows::Forms::MessageBoxIcon::Warning));
		else// == 2
			r = (System::Windows::Forms::MessageBox::Show("Are you sure that you would like \nto delete both of the selected products?", " Delete Product",
		System::Windows::Forms::MessageBoxButtons::YesNo,
		System::Windows::Forms::MessageBoxIcon::Warning));
		if(r == System::Windows::Forms::DialogResult::Yes)
			;//del all
		else if(r == System::Windows::Forms::DialogResult::No)
			return;//quit the loop directly
	}
	for(int i = 0; i < this->list_lv->SelectedItems->Count; i++){
		if(case_tooMany == Yes_Yes || (System::Windows::Forms::MessageBox::Show("Are you sure that you would like \nto delete this product, " +
			this->Get_sName(i) + " - " + 
			this->Get_sBarcode(i) +
			"?", " Delete Product",
		System::Windows::Forms::MessageBoxButtons::YesNo,
		System::Windows::Forms::MessageBoxIcon::Warning)) == System::Windows::Forms::DialogResult::Yes){
				if(Bridging->Del(this->list_lv->SelectedItems[i]))
				{
					this->Clear_selectedItem(i--);//if delete an item, selectedItems->Count will decrease, index will change as well
					this->Update_statusBar(deleteS);
				}
				else
					this->Update_statusBar(deleteF);
		}
		else
			return;
	}
	this->Toggle_list_b(false);
}
//Function: toggle 'enabled' properties for pd_b_delete, pd_b_sell, pd_b_restock buttons
void mainForm::Toggle_list_b(bool tof){
	this->list_b_delete->Enabled = tof;
	this->list_b_sell->Enabled = tof;
	this->list_b_restock->Enabled = tof;
	
}
//Event: when select an item in the list, update all pd_tB textBoxes by using this item's properties.
void mainForm::list_lv_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	this->list_lv->SelectedItems->Count != 0 ? this->Toggle_list_b(true): this->Toggle_list_b(false);
}
//Event: when click the column in the list, sort it
void mainForm::list_lv_ColumnClick(System::Object^, System::Windows::Forms::ColumnClickEventArgs^ e){
	if(this->list_lv->Items->Count > 0)
	{
		bool is_num = InputCheck::is_number(this->list_lv->Items[0]->SubItems[e->Column]->Text);
		if(e->Column != this->list_sortColumn){// check whether it clicks the same column
			this->list_sortColumn = e->Column;
			this->Sort_list_lv(e, Ascending, is_num);
		}
		else{
			if(this->list_sort == Ascending){
				this->Sort_list_lv(e, Descending, is_num);
			}
			else{
				this->Sort_list_lv(e, Ascending, is_num);
			}
		}
	}

}
//Function: set the sort status, and then sort the list
void mainForm::Sort_list_lv(System::Windows::Forms::ColumnClickEventArgs^ e, bool t, bool is_num){
	this->list_sort = t;// sort status: Ascending or Descending
	this->list_lv->ListViewItemSorter = gcnew ListViewItemComparer(e->Column, t, is_num);// sort it
}
//Function: clear the selected item in the list
void mainForm::Clear_selectedItem(int index){
	this->list_lv->SelectedItems[index]->Remove();
}
//
void mainForm::Update_selectedItem_sell(int index, unsigned num){
	int no_stock = System::Convert::ToInt32(this->list_lv->SelectedItems[index]->SubItems[5]->Text) - num,
		no_sold = System::Convert::ToInt32(this->list_lv->SelectedItems[index]->SubItems[6]->Text) + num;
	this->list_lv->SelectedItems[index]->SubItems[5]->Text = no_stock.ToString();
	this->list_lv->SelectedItems[index]->SubItems[6]->Text = no_sold.ToString();
}
//
void mainForm::Update_selectedItem_restock(int index, unsigned num){
	int no_stock = System::Convert::ToInt32(this->list_lv->SelectedItems[index]->SubItems[5]->Text) + num;
	this->list_lv->SelectedItems[index]->SubItems[5]->Text = no_stock.ToString();
}
//Function: get selected item's barcode number
System::String^ mainForm::Get_sBarcode(int index){
	return this->list_lv->SelectedItems[index]->SubItems[2]->Text;
}
//Function: get selected item's name
System::String^ mainForm::Get_sName(int index){
	if(this->list_lv->SelectedItems[index]->SubItems[0]->Text->Length > 15)
		return this->list_lv->SelectedItems[index]->SubItems[0]->Text->Substring(0,15) + "...";
	return this->list_lv->SelectedItems[index]->SubItems[0]->Text;
}

//**************************************************
//**********STATUSBAR COMPONENTS FUNCTION***********
//**************************************************

//Function: update statusBar's Text and BackColor
void mainForm::Update_statusBar(int i){
	array<System::String^>^ text = gcnew array<System::String^>{
		"Product(s) added successfully", "Product(s) added unsuccessfully", //addS , addF
			"Data loaded successfully", "Data loaded unsuccessfully", //loadS, loadF
			"Data saved successfully", "Data saved unsuccessfully", //saveS, saveF
			"Product(s) sold successfully", "Product(s) sold unsuccessfully", //sellS, sellF
			"Product(s) restocked successfully", "Product(s) restocked unsuccessfully", //restockS, restockF
			"Product(s) deleted successfully", "Product(s) deleted unsuccessfully", //deleteS, deleteF
			"Price discounted successfully", "Price discounted unsuccessfully", //discountS, discountF
			"Searched successfully", "No results found" //searchS, searchF
	};
	this->Set_statusBar(text[i], i % 2? /*failure*/System::Drawing::Color::RosyBrown: /*success*/System::Drawing::Color::LightSkyBlue);
}
//Function: set statusBus's Text and BackColor
void mainForm::Set_statusBar(System::String^ s, System::Drawing::Color c){
	this->toolStripStatusLabel1->Text = s;
	this->statusStrip1->BackColor = c;
}

//**********************************************
//**********OTHER COMPONENTS FUNCTION***********
//**********************************************

//Initialize the components & set their properties; run at startup of class mainForm
void mainForm::InitializeComponent()
{
	this->menu = (gcnew System::Windows::Forms::MenuStrip());
	this->menu_f = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->menu_f_addNewProducts = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
	this->menu_f_quit = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->menu_stat = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->menu_stat_BSpd = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->menu_stat_BSmanu = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->menu_stat_topXpd = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->menu_about = (gcnew System::Windows::Forms::ToolStripMenuItem());
	this->s_tB_input = (gcnew System::Windows::Forms::TextBox());
	this->s_b_submit = (gcnew System::Windows::Forms::Button());
	this->s_grp = (gcnew System::Windows::Forms::GroupBox());
	this->s_rB_byManufacturer = (gcnew System::Windows::Forms::RadioButton());
	this->s_l_by = (gcnew System::Windows::Forms::Label());
	this->s_rB_byCategory = (gcnew System::Windows::Forms::RadioButton());
	this->s_rB_byBarcode = (gcnew System::Windows::Forms::RadioButton());
	this->s_rB_byName = (gcnew System::Windows::Forms::RadioButton());
	this->list_b_restock = (gcnew System::Windows::Forms::Button());
	this->list_b_sell = (gcnew System::Windows::Forms::Button());
	this->list_b_delete = (gcnew System::Windows::Forms::Button());
	this->list_lv = (gcnew System::Windows::Forms::ListView());
	this->list_col_name = (gcnew System::Windows::Forms::ColumnHeader());
	this->list_col_category = (gcnew System::Windows::Forms::ColumnHeader());
	this->list_col_barcode = (gcnew System::Windows::Forms::ColumnHeader());
	this->list_col_price = (gcnew System::Windows::Forms::ColumnHeader());
	this->list_col_manuf = (gcnew System::Windows::Forms::ColumnHeader());
	this->list_col_stock = (gcnew System::Windows::Forms::ColumnHeader());
	this->list_col_sold = (gcnew System::Windows::Forms::ColumnHeader());
	this->list_grp = (gcnew System::Windows::Forms::GroupBox());
	this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
	this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
	this->menu->SuspendLayout();
	this->s_grp->SuspendLayout();
	this->list_grp->SuspendLayout();
	this->statusStrip1->SuspendLayout();
	this->SuspendLayout();
	// 
	// menu
	// 
	this->menu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->menu_f, this->menu_stat, 
		this->menu_about});
	this->menu->Location = System::Drawing::Point(0, 0);
	this->menu->Name = L"menu";
	this->menu->Size = System::Drawing::Size(845, 24);
	this->menu->TabIndex = 15;
	this->menu->Text = L"menu";
	// 
	// menu_f
	// 
	this->menu_f->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->menu_f_addNewProducts, 
		this->toolStripSeparator1, this->menu_f_quit});
	this->menu_f->Name = L"menu_f";
	this->menu_f->Size = System::Drawing::Size(37, 20);
	this->menu_f->Text = L"File";
	// 
	// menu_f_addNewProducts
	// 
	this->menu_f_addNewProducts->Name = L"menu_f_addNewProducts";
	this->menu_f_addNewProducts->Size = System::Drawing::Size(166, 22);
	this->menu_f_addNewProducts->Text = L"Add new products";
	this->menu_f_addNewProducts->Click += gcnew System::EventHandler(this, &mainForm::menu_f_addNewProducts_Click);
	// 
	// toolStripSeparator1
	// 
	this->toolStripSeparator1->Name = L"toolStripSeparator1";
	this->toolStripSeparator1->Size = System::Drawing::Size(163, 6);
	// 
	// menu_f_quit
	// 
	this->menu_f_quit->Name = L"menu_f_quit";
	this->menu_f_quit->Size = System::Drawing::Size(166, 22);
	this->menu_f_quit->Text = L"Quit";
	this->menu_f_quit->Click += gcnew System::EventHandler(this, &mainForm::menu_f_quit_Click);
	// 
	// menu_stat
	// 
	this->menu_stat->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->menu_stat_BSpd, 
		this->menu_stat_BSmanu, this->menu_stat_topXpd});
	this->menu_stat->Name = L"menu_stat";
	this->menu_stat->Size = System::Drawing::Size(64, 20);
	this->menu_stat->Text = L"Statistics";
	// 
	// menu_stat_BSpd
	// 
	this->menu_stat_BSpd->Name = L"menu_stat_BSpd";
	this->menu_stat_BSpd->Size = System::Drawing::Size(261, 22);
	this->menu_stat_BSpd->Text = L"Report the Best-Selling product";
	this->menu_stat_BSpd->Click += gcnew System::EventHandler(this, &mainForm::menu_stat_BSpd_Click);
	// 
	// menu_stat_BSmanu
	// 
	this->menu_stat_BSmanu->Name = L"menu_stat_BSmanu";
	this->menu_stat_BSmanu->Size = System::Drawing::Size(261, 22);
	this->menu_stat_BSmanu->Text = L"Report the Best-Selling manufacturer";
	this->menu_stat_BSmanu->Click += gcnew System::EventHandler(this, &mainForm::menu_stat_BSmanu_Click);
	// 
	// menu_stat_topXpd
	// 
	this->menu_stat_topXpd->Name = L"menu_stat_topXpd";
	this->menu_stat_topXpd->Size = System::Drawing::Size(261, 22);
	this->menu_stat_topXpd->Text = L"Report the Top X Selling products";
	this->menu_stat_topXpd->Click += gcnew System::EventHandler(this, &mainForm::menu_stat_topXpd_Click);
	// 
	// menu_about
	// 
	this->menu_about->Name = L"menu_about";
	this->menu_about->Size = System::Drawing::Size(50, 20);
	this->menu_about->Text = L"About";
	this->menu_about->Click += gcnew System::EventHandler(this, &mainForm::menu_about_Click);
	// 
	// s_tB_input
	// 
	this->s_tB_input->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(134)));
	this->s_tB_input->Location = System::Drawing::Point(10, 23);
	this->s_tB_input->MaxLength = 21;
	this->s_tB_input->Name = L"s_tB_input";
	this->s_tB_input->Size = System::Drawing::Size(172, 24);
	this->s_tB_input->TabIndex = 0;
	this->s_tB_input->Click += gcnew System::EventHandler(this, &mainForm::s_tB_input_Click);
	this->s_tB_input->TextChanged += gcnew System::EventHandler(this, &mainForm::s_tB_input_TextChanged);
	this->s_tB_input->LostFocus += gcnew System::EventHandler(this, &mainForm::s_tB_input_LostFocus);
	// 
	// s_b_submit
	// 
	this->s_b_submit->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
		static_cast<System::Byte>(134)));
	this->s_b_submit->Location = System::Drawing::Point(188, 23);
	this->s_b_submit->Name = L"s_b_submit";
	this->s_b_submit->Size = System::Drawing::Size(42, 24);
	this->s_b_submit->TabIndex = 1;
	this->s_b_submit->Text = L"Go";
	this->s_b_submit->UseVisualStyleBackColor = true;
	this->s_b_submit->Click += gcnew System::EventHandler(this, &mainForm::s_b_submit_Click);
	// 
	// s_grp
	// 
	this->s_grp->Controls->Add(this->s_rB_byManufacturer);
	this->s_grp->Controls->Add(this->s_l_by);
	this->s_grp->Controls->Add(this->s_rB_byCategory);
	this->s_grp->Controls->Add(this->s_rB_byBarcode);
	this->s_grp->Controls->Add(this->s_rB_byName);
	this->s_grp->Controls->Add(this->s_tB_input);
	this->s_grp->Controls->Add(this->s_b_submit);
	this->s_grp->Location = System::Drawing::Point(16, 27);
	this->s_grp->Name = L"s_grp";
	this->s_grp->Size = System::Drawing::Size(239, 101);
	this->s_grp->TabIndex = 14;
	this->s_grp->TabStop = false;
	this->s_grp->Text = L"Search";
	// 
	// s_rB_byManufacturer
	// 
	this->s_rB_byManufacturer->AutoSize = true;
	this->s_rB_byManufacturer->Location = System::Drawing::Point(109, 75);
	this->s_rB_byManufacturer->Name = L"s_rB_byManufacturer";
	this->s_rB_byManufacturer->Size = System::Drawing::Size(85, 17);
	this->s_rB_byManufacturer->TabIndex = 8;
	this->s_rB_byManufacturer->TabStop = true;
	this->s_rB_byManufacturer->Text = L"Manufacturer";
	this->s_rB_byManufacturer->UseVisualStyleBackColor = true;
	this->s_rB_byManufacturer->CheckedChanged += gcnew System::EventHandler(this, &mainForm::s_rB_byManufacturer_CheckedChanged);
	// 
	// s_l_by
	// 
	this->s_l_by->AutoSize = true;
	this->s_l_by->Location = System::Drawing::Point(15, 55);
	this->s_l_by->Name = L"s_l_by";
	this->s_l_by->Size = System::Drawing::Size(19, 13);
	this->s_l_by->TabIndex = 7;
	this->s_l_by->Text = L"By";
	// 
	// s_rB_byCategory
	// 
	this->s_rB_byCategory->AutoSize = true;
	this->s_rB_byCategory->Location = System::Drawing::Point(109, 54);
	this->s_rB_byCategory->Name = L"s_rB_byCategory";
	this->s_rB_byCategory->Size = System::Drawing::Size(64, 17);
	this->s_rB_byCategory->TabIndex = 4;
	this->s_rB_byCategory->TabStop = true;
	this->s_rB_byCategory->Text = L"Category";
	this->s_rB_byCategory->UseVisualStyleBackColor = true;
	this->s_rB_byCategory->CheckedChanged += gcnew System::EventHandler(this, &mainForm::s_rB_byCategory_CheckedChanged);
	// 
	// s_rB_byBarcode
	// 
	this->s_rB_byBarcode->AutoSize = true;
	this->s_rB_byBarcode->Location = System::Drawing::Point(39, 75);
	this->s_rB_byBarcode->Name = L"s_rB_byBarcode";
	this->s_rB_byBarcode->Size = System::Drawing::Size(62, 17);
	this->s_rB_byBarcode->TabIndex = 3;
	this->s_rB_byBarcode->TabStop = true;
	this->s_rB_byBarcode->Text = L"Barcode";
	this->s_rB_byBarcode->UseVisualStyleBackColor = true;
	this->s_rB_byBarcode->CheckedChanged += gcnew System::EventHandler(this, &mainForm::s_rB_byBarcode_CheckedChanged);
	// 
	// s_rB_byName
	// 
	this->s_rB_byName->AutoSize = true;
	this->s_rB_byName->Checked = true;
	this->s_rB_byName->Location = System::Drawing::Point(39, 54);
	this->s_rB_byName->Name = L"s_rB_byName";
	this->s_rB_byName->Size = System::Drawing::Size(50, 17);
	this->s_rB_byName->TabIndex = 2;
	this->s_rB_byName->TabStop = true;
	this->s_rB_byName->Text = L"Name";
	this->s_rB_byName->UseVisualStyleBackColor = true;
	this->s_rB_byName->CheckedChanged += gcnew System::EventHandler(this, &mainForm::s_rB_byName_CheckedChanged);
	// 
	// list_b_restock
	// 
	this->list_b_restock->Enabled = false;
	this->list_b_restock->Location = System::Drawing::Point(395, 339);
	this->list_b_restock->Name = L"list_b_restock";
	this->list_b_restock->Size = System::Drawing::Size(75, 23);
	this->list_b_restock->TabIndex = 9;
	this->list_b_restock->Text = L"Restock";
	this->list_b_restock->UseVisualStyleBackColor = true;
	this->list_b_restock->Click += gcnew System::EventHandler(this, &mainForm::list_b_restock_Click);
	// 
	// list_b_sell
	// 
	this->list_b_sell->Enabled = false;
	this->list_b_sell->Location = System::Drawing::Point(314, 339);
	this->list_b_sell->Name = L"list_b_sell";
	this->list_b_sell->Size = System::Drawing::Size(75, 23);
	this->list_b_sell->TabIndex = 8;
	this->list_b_sell->Text = L"Sell";
	this->list_b_sell->UseVisualStyleBackColor = true;
	this->list_b_sell->Click += gcnew System::EventHandler(this, &mainForm::list_b_sell_Click);
	// 
	// list_b_delete
	// 
	this->list_b_delete->Enabled = false;
	this->list_b_delete->Location = System::Drawing::Point(476, 339);
	this->list_b_delete->Name = L"list_b_delete";
	this->list_b_delete->Size = System::Drawing::Size(75, 23);
	this->list_b_delete->TabIndex = 11;
	this->list_b_delete->Text = L"Delete";
	this->list_b_delete->UseVisualStyleBackColor = true;
	this->list_b_delete->Click += gcnew System::EventHandler(this, &mainForm::list_b_delete_Click);
	// 
	// list_lv
	// 
	this->list_lv->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(7) {this->list_col_name, this->list_col_category, 
		this->list_col_barcode, this->list_col_price, this->list_col_manuf, this->list_col_stock, this->list_col_sold});
	this->list_lv->FullRowSelect = true;
	this->list_lv->Location = System::Drawing::Point(11, 23);
	this->list_lv->Name = L"list_lv";
	this->list_lv->ShowGroups = false;
	this->list_lv->Size = System::Drawing::Size(539, 306);
	this->list_lv->TabIndex = 7;
	this->list_lv->UseCompatibleStateImageBehavior = false;
	this->list_lv->View = System::Windows::Forms::View::Details;
	this->list_lv->ColumnClick += gcnew System::Windows::Forms::ColumnClickEventHandler(this, &mainForm::list_lv_ColumnClick);
	this->list_lv->SelectedIndexChanged += gcnew System::EventHandler(this, &mainForm::list_lv_SelectedIndexChanged);
	// 
	// list_col_name
	// 
	this->list_col_name->Text = L"Name";
	this->list_col_name->Width = 110;
	// 
	// list_col_category
	// 
	this->list_col_category->Text = L"Category";
	this->list_col_category->Width = 61;
	// 
	// list_col_barcode
	// 
	this->list_col_barcode->Text = L"Barcode";
	this->list_col_barcode->Width = 86;
	// 
	// list_col_price
	// 
	this->list_col_price->Text = L"Price";
	// 
	// list_col_manuf
	// 
	this->list_col_manuf->Text = L"Manufacturer";
	this->list_col_manuf->Width = 82;
	// 
	// list_col_stock
	// 
	this->list_col_stock->Text = L"Stock";
	// 
	// list_col_sold
	// 
	this->list_col_sold->Text = L"Sold";
	// 
	// list_grp
	// 
	this->list_grp->Controls->Add(this->list_lv);
	this->list_grp->Controls->Add(this->list_b_restock);
	this->list_grp->Controls->Add(this->list_b_delete);
	this->list_grp->Controls->Add(this->list_b_sell);
	this->list_grp->Location = System::Drawing::Point(269, 27);
	this->list_grp->Name = L"list_grp";
	this->list_grp->Size = System::Drawing::Size(561, 373);
	this->list_grp->TabIndex = 13;
	this->list_grp->TabStop = false;
	this->list_grp->Text = L"Result";
	// 
	// statusStrip1
	// 
	this->statusStrip1->BackColor = System::Drawing::Color::LightSkyBlue;
	this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->toolStripStatusLabel1});
	this->statusStrip1->Location = System::Drawing::Point(0, 409);
	this->statusStrip1->Name = L"statusStrip1";
	this->statusStrip1->Size = System::Drawing::Size(845, 22);
	this->statusStrip1->SizingGrip = false;
	this->statusStrip1->TabIndex = 12;
	this->statusStrip1->Text = L"statusStrip";
	// 
	// toolStripStatusLabel1
	// 
	this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
	this->toolStripStatusLabel1->Size = System::Drawing::Size(39, 17);
	this->toolStripStatusLabel1->Text = L"Ready";
	// 
	// mainForm
	// 
	this->AcceptButton = this->s_b_submit;
	this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
	this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
	this->ClientSize = System::Drawing::Size(845, 431);
	this->Controls->Add(this->statusStrip1);
	this->Controls->Add(this->list_grp);
	this->Controls->Add(this->s_grp);
	this->Controls->Add(this->menu);
	this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	this->MainMenuStrip = this->menu;
	this->MaximizeBox = false;
	this->Name = L"mainForm";
	this->Text = L" CICMS";
	this->menu->ResumeLayout(false);
	this->menu->PerformLayout();
	this->s_grp->ResumeLayout(false);
	this->s_grp->PerformLayout();
	this->list_grp->ResumeLayout(false);
	this->statusStrip1->ResumeLayout(false);
	this->statusStrip1->PerformLayout();
	this->ResumeLayout(false);
	this->PerformLayout();

}