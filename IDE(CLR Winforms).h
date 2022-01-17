		Point lastPoint;
		private: System::Void guna2Panel1_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
			lastPoint = Point(e->X, e->Y);
		}
		private: System::Void guna2Panel1_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
			if (e->Button == System::Windows::Forms::MouseButtons::Left)
			{
				this->Left += e->X - lastPoint.X;
				this->Top += e->Y - lastPoint.Y;
			}
		}
		private: System::Void PauseSleep() {
			Random^ rand = gcnew Random();
			int number = rand->Next(2000, 5000);
			Thread::Sleep(number);
		}
		private: System::Void UpdateTextBox() {
			guna2TextBox2->Update();
			guna2TextBox2->SelectionStart = guna2TextBox2->Text->Length;
			guna2TextBox2->ScrollToCaret();
		}
			   String^ title = String::Empty;
			   String^ description = String::Empty;
			   String^ company = String::Empty;
			   String^ product = String::Empty;
			   String^ copyright = String::Empty;
			   String^ trademark = String::Empty;
			   String^ version = String::Empty;


		private: System::Void guna2Button2_Click(System::Object^ sender, System::EventArgs^ e) {
			Stream^ myStream;
			SaveFileDialog^ saveFileDialog1 = gcnew SaveFileDialog();

			saveFileDialog1->Filter = "exe files (*.exe)|*.exe";//|All files (*.*)|*.*";
			saveFileDialog1->FileName = "output.exe";
			saveFileDialog1->FilterIndex = 2;
			saveFileDialog1->RestoreDirectory = true;

			String^ outputName = saveFileDialog1->FileName;
			if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				outputName = saveFileDialog1->FileName;

				String^ src = gcnew String(rawsource);

				guna2TextBox2->Text = guna2TextBox2->Text + Environment::NewLine + "Compiling Assembly...";
				UpdateTextBox();
				PauseSleep();
				try {
					src = src->Replace("%Title%", title);
					src = src->Replace("%Description%", description);
					src = src->Replace("%Company%", company);
					src = src->Replace("%Product%", product);
					src = src->Replace("%Copyright%", copyright);
					src = src->Replace("%Version%", version);
				}
				catch(Exception^)
				{
				}

				guna2TextBox2->Text = guna2TextBox2->Text + Environment::NewLine + "Compiling EXE...";
				UpdateTextBox();
				PauseSleep();
				CodeDomProvider^ codeProvider = CodeDomProvider::CreateProvider("CSharp");
				CompilerParameters^ parameters = gcnew CompilerParameters();
				parameters->GenerateExecutable = true;
				parameters->OutputAssembly = outputName;


				parameters->ReferencedAssemblies->Add("System.dll");
				parameters->ReferencedAssemblies->Add("System.Drawing.dll");
				parameters->ReferencedAssemblies->Add("System.Net.dll");
				parameters->ReferencedAssemblies->Add("System.Net.Http.dll");
				parameters->ReferencedAssemblies->Add("System.Windows.Forms.dll");
				parameters->ReferencedAssemblies->Add("mscorlib.dll");

				CompilerResults^ results = codeProvider->CompileAssemblyFromSource(parameters, src);

				if (results->Errors->Count > 0)
				{
					for each (CompilerError ^ CompErr in results->Errors)
					{
						guna2TextBox2->Text = guna2TextBox2->Text + Environment::NewLine +
							"Line number " + CompErr->Line +
							", Error Number: " + CompErr->ErrorNumber +
							", '" + CompErr->ErrorText + ";";
					}
					guna2TextBox2->Text = guna2TextBox2->Text + Environment::NewLine + "An error has occured when trying to compile file.";
					UpdateTextBox();
				}
				else
				{
					guna2TextBox2->Text = guna2TextBox2->Text + Environment::NewLine + "Successfully compiled file!";
					UpdateTextBox();
				}
			}
			else {
				guna2TextBox2->Text = "Error compiling file, please make sure to set the file location and name";
			}
		}

		private: System::Void Builder_Load(System::Object^ sender, System::EventArgs^ e) {
		}
			   Boolean assemblyopened;
			   AssemblyManager^ asmform;
		private: System::Void AssemblyManager_FormClosed(System::Object^ sender, System::Windows::Forms::FormClosedEventArgs^ e) {
			assemblyopened = false;
		}
		private: System::Void AssemblyManager_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e) {
			title = asmform->titleBox->Text;
			description = asmform->descBox->Text;
			company = asmform->compBox->Text;
			product = asmform->productBox->Text;
			copyright = asmform->copyrightBox->Text;
			trademark = asmform->trademarkBox->Text;
			version = asmform->versionBox->Text;
		}

		private: System::Void guna2Button3_Click(System::Object^ sender, System::EventArgs^ e) {
			if (assemblyopened == false) {
				AssemblyManager^ AssemblyMng = gcnew AssemblyManager();
				AssemblyMng->Show();
				assemblyopened = true;
				asmform = AssemblyMng;
				AssemblyMng->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Builder::AssemblyManager_FormClosing);
				AssemblyMng->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Builder::AssemblyManager_FormClosed);
			}
		}
