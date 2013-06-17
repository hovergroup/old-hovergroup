object MainForm: TMainForm
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'STRSVR'
  ClientHeight = 212
  ClientWidth = 384
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel3: TPanel
    Left = 0
    Top = 0
    Width = 384
    Height = 189
    Align = alTop
    BevelOuter = bvNone
    BorderWidth = 1
    TabOrder = 4
    object Panel1: TPanel
      Left = 1
      Top = 27
      Width = 382
      Height = 128
      Align = alTop
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 0
      object Output3Bps: TLabel
        Left = 308
        Top = 104
        Width = 57
        Height = 13
        Alignment = taRightJustify
        AutoSize = False
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Output2Bps: TLabel
        Left = 308
        Top = 81
        Width = 57
        Height = 13
        Alignment = taRightJustify
        AutoSize = False
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Output1Bps: TLabel
        Left = 308
        Top = 58
        Width = 57
        Height = 13
        Alignment = taRightJustify
        AutoSize = False
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object InputBps: TLabel
        Left = 308
        Top = 24
        Width = 57
        Height = 13
        Alignment = taRightJustify
        AutoSize = False
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object LabelInput: TLabel
        Left = 30
        Top = 24
        Width = 43
        Height = 13
        Caption = '(0) Input'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object LabelOutput1: TLabel
        Left = 30
        Top = 58
        Width = 51
        Height = 13
        Caption = '(1) Output'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label3: TLabel
        Left = 122
        Top = 4
        Width = 24
        Height = 13
        Caption = 'Type'
      end
      object Label4: TLabel
        Left = 182
        Top = 4
        Width = 18
        Height = 13
        Caption = 'Opt'
      end
      object InputByte: TLabel
        Left = 228
        Top = 24
        Width = 77
        Height = 13
        Alignment = taRightJustify
        AutoSize = False
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label6: TLabel
        Left = 278
        Top = 4
        Width = 27
        Height = 13
        Caption = 'bytes'
      end
      object Label7: TLabel
        Left = 348
        Top = 4
        Width = 17
        Height = 13
        Caption = 'bps'
      end
      object LabelOutput2: TLabel
        Left = 30
        Top = 81
        Width = 51
        Height = 13
        Caption = '(2) Output'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Output1Byte: TLabel
        Left = 228
        Top = 58
        Width = 77
        Height = 13
        Alignment = taRightJustify
        AutoSize = False
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Output2Byte: TLabel
        Left = 228
        Top = 81
        Width = 77
        Height = 13
        Alignment = taRightJustify
        AutoSize = False
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label5: TLabel
        Left = 42
        Top = 4
        Width = 34
        Height = 13
        Caption = 'Stream'
      end
      object LabelOutput3: TLabel
        Left = 30
        Top = 104
        Width = 51
        Height = 13
        Caption = '(3) Output'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Output3Byte: TLabel
        Left = 228
        Top = 104
        Width = 77
        Height = 13
        Alignment = taRightJustify
        AutoSize = False
        Caption = '0'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label1: TLabel
        Left = 202
        Top = 4
        Width = 21
        Height = 13
        Caption = 'Cmd'
      end
      object Label2: TLabel
        Left = 200
        Top = 41
        Width = 25
        Height = 13
        Caption = 'Conv'
      end
      object Input: TComboBox
        Left = 91
        Top = 20
        Width = 89
        Height = 21
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 1
        Text = 'Serial'
        OnChange = InputChange
        Items.Strings = (
          'Serial'
          'TCP Client'
          'TCP Server'
          'NTRIP Client'
          'File'
          'FTP'
          'HTTP')
      end
      object Output1: TComboBox
        Left = 91
        Top = 54
        Width = 89
        Height = 21
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 5
        OnChange = Output1Change
        Items.Strings = (
          ''
          'Serial'
          'TCP Client'
          'TCP Server'
          'NTRIP Server'
          'File')
      end
      object BtnInput: TButton
        Left = 182
        Top = 20
        Width = 19
        Height = 21
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -9
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
        OnClick = BtnInputClick
      end
      object BtnOutput1: TButton
        Left = 182
        Top = 54
        Width = 19
        Height = 21
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -9
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 6
        OnClick = BtnOutput1Click
      end
      object BtnOutput2: TButton
        Left = 182
        Top = 77
        Width = 19
        Height = 21
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -9
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 9
        OnClick = BtnOutput2Click
      end
      object IndInput: TPanel
        Left = 10
        Top = 25
        Width = 12
        Height = 12
        BevelInner = bvRaised
        BevelOuter = bvLowered
        ParentBackground = False
        TabOrder = 0
      end
      object Output2: TComboBox
        Left = 91
        Top = 77
        Width = 89
        Height = 21
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 8
        OnChange = Output2Change
        Items.Strings = (
          ''
          'Serial'
          'TCP Client'
          'TCP Server'
          'NTRIP Server'
          'File')
      end
      object IndOutput1: TPanel
        Left = 10
        Top = 59
        Width = 12
        Height = 12
        BevelInner = bvRaised
        BevelOuter = bvLowered
        ParentBackground = False
        TabOrder = 4
      end
      object IndOutput2: TPanel
        Left = 10
        Top = 82
        Width = 12
        Height = 12
        BevelInner = bvRaised
        BevelOuter = bvLowered
        ParentBackground = False
        TabOrder = 7
      end
      object Output3: TComboBox
        Left = 91
        Top = 100
        Width = 89
        Height = 21
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 11
        OnChange = Output3Change
        Items.Strings = (
          ''
          'Serial'
          'TCP Client'
          'TCP Server'
          'NTRIP Server'
          'File')
      end
      object BtnOutput3: TButton
        Left = 182
        Top = 100
        Width = 19
        Height = 21
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -9
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 12
        OnClick = BtnOutput3Click
      end
      object IndOutput3: TPanel
        Left = 10
        Top = 105
        Width = 12
        Height = 12
        BevelInner = bvRaised
        BevelOuter = bvLowered
        ParentBackground = False
        TabOrder = 10
      end
      object BtnCmd: TButton
        Left = 202
        Top = 20
        Width = 19
        Height = 21
        Caption = '...'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -9
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
        OnClick = BtnCmdClick
      end
      object BtnConv1: TButton
        Left = 202
        Top = 54
        Width = 19
        Height = 21
        Caption = '...'
        Enabled = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -9
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 13
        OnClick = BtnConv1Click
      end
      object BtnConv2: TButton
        Left = 202
        Top = 77
        Width = 19
        Height = 21
        Caption = '...'
        Enabled = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -9
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 14
        OnClick = BtnConv2Click
      end
      object BtnConv3: TButton
        Left = 202
        Top = 100
        Width = 19
        Height = 21
        Caption = '...'
        Enabled = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -9
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 15
        OnClick = BtnConv3Click
      end
    end
    object Panel4: TPanel
      Left = 1
      Top = 162
      Width = 382
      Height = 26
      Align = alClient
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 1
      object Message: TLabel
        Left = 4
        Top = 5
        Width = 372
        Height = 13
        Alignment = taCenter
        AutoSize = False
        Caption = 'message area'
        Color = clBtnFace
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentColor = False
        ParentFont = False
      end
      object BtnAbout: TSpeedButton
        Left = 365
        Top = 5
        Width = 11
        Height = 17
        Caption = '?'
        Flat = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        OnClick = BtnAboutClick
      end
      object BtnStrMon: TSpeedButton
        Left = 4
        Top = 4
        Width = 17
        Height = 17
        Hint = 'Stream Monitor'
        Flat = True
        Glyph.Data = {
          3E020000424D3E0200000000000036000000280000000D0000000D0000000100
          1800000000000802000000000000000000000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFF8080808080
          80808080808080808080808080808080FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFF
          FFFFFF808080FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF808080FFFFFFFFFFFFFFFF
          FF00FFFFFFFFFFFFFFFFFF808080FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF808080
          FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFF808080FFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFF808080FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFF808080FFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFF808080FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFF
          FFFFFF808080FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF808080FFFFFFFFFFFFFFFF
          FF00FFFFFFFFFFFFFFFFFF808080808080808080808080808080808080808080
          FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FF00}
        ParentShowHint = False
        ShowHint = True
        OnClick = BtnStrMonClick
      end
      object BtnTaskIcon: TSpeedButton
        Left = 347
        Top = 5
        Width = 17
        Height = 17
        Hint = 'Task Tray Icon'
        Flat = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        Glyph.Data = {
          3E020000424D3E0200000000000036000000280000000D0000000D0000000100
          1800000000000802000000000000000000000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFF787878FFFFFF787878FFFFFF787878FF
          FFFF000000000000000000FFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFF000000FFFFFF000000FFFFFFFFFFFF00FFFFFFFFFFFF
          787878FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000FFFFFFFFFF
          FF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFF787878FFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFF787878FFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFF
          787878FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF787878FFFFFFFFFF
          FF00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFF00FFFFFFFFFFFF787878FFFFFF787878FFFFFF787878FF
          FFFF787878FFFFFF787878FFFFFFFFFFFF00FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FF00}
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        OnClick = BtnTaskIconClick
      end
    end
    object Progress: TProgressBar
      Left = 1
      Top = 155
      Width = 382
      Height = 7
      Align = alTop
      Smooth = True
      Step = 1
      TabOrder = 2
    end
    object Panel2: TPanel
      Left = 1
      Top = 1
      Width = 382
      Height = 26
      Align = alTop
      BevelInner = bvRaised
      BevelOuter = bvLowered
      TabOrder = 3
      object Label8: TLabel
        Left = 197
        Top = 5
        Width = 81
        Height = 14
        Caption = 'Connect Time:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -12
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object ConTime: TLabel
        Left = 298
        Top = 5
        Width = 68
        Height = 14
        Alignment = taRightJustify
        Caption = '0d 00:00:00'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -12
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Time: TLabel
        Left = 10
        Top = 5
        Width = 154
        Height = 14
        Caption = '2010/01/01 00:00:00 GPST'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clGray
        Font.Height = -12
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
    end
  end
  object BtnStart: TButton
    Left = 1
    Top = 188
    Width = 91
    Height = 23
    Caption = '&Start'
    TabOrder = 0
    OnClick = BtnStartClick
  end
  object BtnStop: TButton
    Left = 92
    Top = 188
    Width = 91
    Height = 23
    Caption = 'S&top'
    Enabled = False
    TabOrder = 1
    OnClick = BtnStopClick
  end
  object BtnOpt: TButton
    Left = 201
    Top = 188
    Width = 91
    Height = 23
    Caption = '&Options...'
    TabOrder = 2
    OnClick = BtnOptClick
  end
  object BtnExit: TButton
    Left = 292
    Top = 188
    Width = 91
    Height = 23
    Caption = 'E&xit'
    TabOrder = 3
    OnClick = BtnExitClick
  end
  object Timer1: TTimer
    Interval = 50
    OnTimer = Timer1Timer
    Left = 198
    Top = 152
  end
  object Timer2: TTimer
    Interval = 100
    OnTimer = Timer2Timer
    Left = 228
    Top = 152
  end
  object PopupMenu: TPopupMenu
    Left = 316
    Top = 152
    object MenuExpand: TMenuItem
      Caption = 'E&xpand'
      OnClick = MenuExpandClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object MenuStart: TMenuItem
      Caption = '&Start'
      OnClick = MenuStartClick
    end
    object MenuStop: TMenuItem
      Caption = '&Stop'
      Enabled = False
      OnClick = MenuStopClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object MenuExit: TMenuItem
      Caption = '&Exit'
      OnClick = MenuExitClick
    end
  end
  object TrayIcon: TTrayIcon
    Hint = 'Stream Server'
    Icon.Data = {
      0000010001001010000001000800680500001600000028000000100000002000
      0000010008000000000040010000000000000000000000000000000000000000
      0000000080000080000000808000800000008000800080800000C0C0C000C0DC
      C000F0CAA6000020400000206000002080000020A0000020C0000020E0000040
      0000004020000040400000406000004080000040A0000040C0000040E0000060
      0000006020000060400000606000006080000060A0000060C0000060E0000080
      0000008020000080400000806000008080000080A0000080C0000080E00000A0
      000000A0200000A0400000A0600000A0800000A0A00000A0C00000A0E00000C0
      000000C0200000C0400000C0600000C0800000C0A00000C0C00000C0E00000E0
      000000E0200000E0400000E0600000E0800000E0A00000E0C00000E0E0004000
      0000400020004000400040006000400080004000A0004000C0004000E0004020
      0000402020004020400040206000402080004020A0004020C0004020E0004040
      0000404020004040400040406000404080004040A0004040C0004040E0004060
      0000406020004060400040606000406080004060A0004060C0004060E0004080
      0000408020004080400040806000408080004080A0004080C0004080E00040A0
      000040A0200040A0400040A0600040A0800040A0A00040A0C00040A0E00040C0
      000040C0200040C0400040C0600040C0800040C0A00040C0C00040C0E00040E0
      000040E0200040E0400040E0600040E0800040E0A00040E0C00040E0E0008000
      0000800020008000400080006000800080008000A0008000C0008000E0008020
      0000802020008020400080206000802080008020A0008020C0008020E0008040
      0000804020008040400080406000804080008040A0008040C0008040E0008060
      0000806020008060400080606000806080008060A0008060C0008060E0008080
      0000808020008080400080806000808080008080A0008080C0008080E00080A0
      000080A0200080A0400080A0600080A0800080A0A00080A0C00080A0E00080C0
      000080C0200080C0400080C0600080C0800080C0A00080C0C00080C0E00080E0
      000080E0200080E0400080E0600080E0800080E0A00080E0C00080E0E000C000
      0000C0002000C0004000C0006000C0008000C000A000C000C000C000E000C020
      0000C0202000C0204000C0206000C0208000C020A000C020C000C020E000C040
      0000C0402000C0404000C0406000C0408000C040A000C040C000C040E000C060
      0000C0602000C0604000C0606000C0608000C060A000C060C000C060E000C080
      0000C0802000C0804000C0806000C0808000C080A000C080C000C080E000C0A0
      0000C0A02000C0A04000C0A06000C0A08000C0A0A000C0A0C000C0A0E000C0C0
      0000C0C02000C0C04000C0C06000C0C08000C0C0A000F0FBFF00A4A0A0008080
      80000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000707
      070707070707070707070707070707FFFFFFFFFFFFFFFFFF00000000000007FF
      FFFFFFFFFFFFFFA400FFFFFFFF0007FFFFFFFFFFFFA4A4A4A4FFFFFFFF0007FF
      FFFFFFFFFFA4FFA400FFFFFFFF00000000000000FFA4FFFF00000000000000FF
      FFFFFF00FFA4FFA400FFFFFFFF0000FFFFFFFF00A4A4A4A4A4FFFFFFFF0000FF
      FFFFFF00FFA4FFA400FFFFFFFF00000000000000FFA4FFFF00000000000007FF
      FFFFFFFFFFA4FFA400FFFFFFFF0007FFFFFFFFFFFFA4A4A4A4FFFFFFFF0007FF
      FFFFFFFFFFFFFFA400FFFFFFFF0007FFFFFFFFFFFFFFFFFF0000000000000707
      0707070707070707070707070707000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFF0000}
    PopupMenu = PopupMenu
    OnDblClick = TrayIconDblClick
    Left = 288
    Top = 152
  end
  object ImageList: TImageList
    Left = 258
    Top = 152
    Bitmap = {
      494C010103000400300010001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000001000000001002000000000000010
      000000000000000000000000000000000000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000B4B4B400B4B4B400B4B4B400B4B4
      B400B4B4B400B4B4B400B4B4B400B4B4B400B4B4B400B4B4B400B4B4B400B4B4
      B400B4B4B400B4B4B400B4B4B400B4B4B4000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      000000000000000000000000000000000000B4B4B400FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF008080800000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0080808000000000000080
      000000800000008000000080000000000000B4B4B400FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00787878000000000080FF
      000080FF000080FF000080FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080808000808080008080800080808000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00808080008080800080808000808080000080
      000000800000008000000080000000000000B4B4B400FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF007878780078787800787878007878780080FF
      000080FF000080FF000080FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080808000FFFFFF008080800000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080808000FFFFFF0080808000000000000080
      000000800000008000000080000000000000B4B4B400FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0078787800FFFFFF00787878000000000080FF
      000080FF000080FF000080FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0080808000FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0080808000FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0078787800FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0000000000FFFFFF0080808000FFFFFF008080800000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000800000008000000080
      00000080000000000000FFFFFF0080808000FFFFFF0080808000000000000080
      0000008000000080000000800000000000000000000080FF000080FF000080FF
      000080FF000000000000FFFFFF0078787800FFFFFF00787878000000000080FF
      000080FF000080FF000080FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00000000008080800080808000808080008080800080808000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000800000008000000080
      0000008000000000000080808000808080008080800080808000808080000080
      0000008000000080000000800000000000000000000080FF000080FF000080FF
      000080FF000000000000787878007878780078787800787878007878780080FF
      000080FF000080FF000080FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0000000000FFFFFF0080808000FFFFFF008080800000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000800000008000000080
      00000080000000000000FFFFFF0080808000FFFFFF0080808000000000000080
      0000008000000080000000800000000000000000000080FF000080FF000080FF
      000080FF000000000000FFFFFF0078787800FFFFFF00787878000000000080FF
      000080FF000080FF000080FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0080808000FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0080808000FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0078787800FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080808000FFFFFF008080800000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080808000FFFFFF0080808000000000000080
      000000800000008000000080000000000000B4B4B400FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0078787800FFFFFF00787878000000000080FF
      000080FF000080FF000080FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0080808000808080008080800080808000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00808080008080800080808000808080000080
      000000800000008000000080000000000000B4B4B400FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF007878780078787800787878007878780080FF
      000080FF000080FF000080FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF008080800000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0080808000000000000080
      000000800000008000000080000000000000B4B4B400FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00787878000000000080FF
      000080FF000080FF000080FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      000000000000000000000000000000000000C0C0C000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      000000000000000000000000000000000000B4B4B400FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000B4B4B400B4B4B400B4B4B400B4B4
      B400B4B4B400B4B4B400B4B4B400B4B4B400B4B4B400B4B4B400B4B4B400B4B4
      B400B4B4B400B4B4B400B4B4B400B4B4B4000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000100000000100010000000000800000000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000FFFFFFFFFFFF000000000000000000000000000000000000
      000000000000}
  end
end
