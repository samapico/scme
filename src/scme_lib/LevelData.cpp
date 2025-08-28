#include "LevelData.h"

#include "ExtraLevelData.h"
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QDebug>


///////////////////////////////////////////////////////////////////////////

using namespace ::SCME;

//////////////////////////////////////////////////////////////////////////

static void Fill(Array2D<Tile>& map, const QRect& area, TileId value)
{
    for (int x = area.left(); x <= area.right(); x++)
        for (int y = area.top(); y <= area.bottom(); y++)
            map(x, y) = value;
}


//////////////////////////////////////////////////////////////////////////

LevelData::LevelData() :
    mIsDirty(false)
{
    mBounds = LevelBounds(LevelCoords(0, 0), QSizeF(size().width() * TILE_W, size().height() * TILE_H));

    mTileset.setDefault();
}

//////////////////////////////////////////////////////////////////////////

LevelData::~LevelData()
{
}

//////////////////////////////////////////////////////////////////////////

bool LevelData::loadFromFile(const QString& filepath)
{
    qDebug() << "Loading level file:" << filepath;

    QFile f(filepath);

    QDataStream in(&f);

    if (f.open(QIODevice::ReadOnly))
    {
        return load(in);

        f.close();
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////

bool LevelData::load(QDataStream& in)
{
    in.setByteOrder(QDataStream::LittleEndian);


    //
    //
    //
    //
    //     Dim b(3) As Byte
    //
    //     Dim f As Integer
    //     f = FreeFile
    //
    //     AddDebug "OpenMap, Opening Map... " & filename & " (" & f & ")"
    //
    //     frmGeneral.IsBusy("frmMain" & id & ".OpenMap") = True
    //
    //     'initialize autosave countdown
    //     MinutesCounted = 0
    //
    //     Call frmGeneral.UpdateProgressLabel("Reading map header...")
    //
    //
    //
    //     Open filename For Binary As #f
    //     Get #f, , b
    //

    bool hasTileset = false;

    ExtraLevelData loadedExtraLevelData;
    Tileset loadedTileset;
    MapTiles loadedTiles;


    //Look for eLVl header
    if (ExtraLevelData::hasELVL(in))
    {
        ExtraLevelData::load(in, loadedExtraLevelData);
    }
    else
    {
        //Look for bitmap header
        //Read first two bytes to see if this level contains a tileset
        unsigned char c;
        in >> c;
        if (c == 'B')
        {
            in >> c;
            if (c == 'M')
            {
                hasTileset = true;
            }
        }
    }

    //Move back to the beginning
    in.device()->seek(0);

    //
    //     If Chr(b(0)) & Chr(b(1)) <> "BM" Then
    //         'there is no tileset attached to the map
    //         'so the default will be used
    //         'default on true
    //         usingDefaultTileset = True
    //
    //         AddDebug "OpenMap, No tileset found ; usingDefaultTileset " & usingDefaultTileset
    //
    //         'update menu
    //         'TORESTORE
    // '        frmGeneral.mnudiscardtileset.Enabled = False
    // '        frmGeneral.mnuExportTileset.Enabled = False
    //         'resize magnifier.zoom tileset
    //         'reset position
    //         Seek #f, 1
    //         'Now we are at the correct position for the tiles
    //     Else


    if (hasTileset)
    {
        qDebug() << "Loading tileset...";

        if (Tileset::load(in, loadedTileset, loadedExtraLevelData))
        {
            if (loadedTileset.image().isNull())
            {
                qWarning() << "No tileset loaded";
                hasTileset = false;
            }

            if (loadedTileset.image().size() != TILESET_SIZE)
            {
                qWarning() << "Invalid tileset dimensions:" << loadedTileset.image().size();
                hasTileset = false;
            }

            qDebug() << "Loaded tileset:" << loadedTileset.image().format() << loadedTileset.fileHeader().bfSize << loadedTileset.infoHeader().biBitCount << loadedTileset.infoHeader().biPlanes << loadedTileset.infoHeader().biSize << loadedTileset.infoHeader().biSizeImage;
        }
        else
        {
            qWarning() << "Error loading tileset:" << in.status();
            hasTileset = false;
        }
    }

    if (!hasTileset)
    {
        qDebug() << "Using default tileset...";
        loadedTileset.setDefault();
    }

    //
    //
    //         'check if eLVL data is present
    //
    //         If LongToUnsigned(bmpFileheader.bfReserved1) > 0 Then
    //             'we most likely have eLVL data
    //
    //             AddDebug "OpenMap, trying to read eLVL data"
    //
    //             eLVL.GetELVLData f, LongToUnsigned(bmpFileheader.bfReserved1), filename
    //
    //             'now seek to the part where the map bytes are stored
    //             'Seek #f, (BMPFileHeader.bfSize + 1)
    //
    //         Else
    //             'probably no eLVL data in that map
    //             AddDebug "OpenMap, no eLVL data found"
    //
    //             'we should already be at the correct place in the file to load tiles
    //         End If
    //
    //
    //         'this is the end of the tileset, tiles should start here
    //
    //         usingDefaultTileset = False
    //         AddDebug "Openmap, usingDefaultTileset " & usingDefaultTileset
    //     End If
    //
    // '          On Local Error GoTo Corrupt_File
    //
    //     'we should already be here
    //     If Seek(f) <> bmpFileheader.bfSize + 1 Then
    //         If bmpFileheader.bfSize = 49720 Then
    //             Seek #f, bmpFileheader.bfSize + 1
    //         End If
    //
    // '        AddDebug "OpenMap, WARNING: Seek(f)=" & Seek(f) & " bfSize+1=" & BMPFileHeader.bfSize + 1
    // '
    // '        Dim startseek As Long
    // '        startseek = Seek(f)
    // '        'Stop
    // '        'something is wrong here, attempt to recovery file
    // '        Dim recoverresult As Long
    // '
    // '        recoverresult = TileDataRecovery(longMinimum(BMPFileHeader.bfSize + 1, startseek), f, 4)
    // ''        recoverresult = TileDataRecovery(BMPFileHeader.bfSize + 1, f, 4)
    // '        If recoverresult < 0 Then
    // '            recoverresult = TileDataRecovery(startseek, f, 4)
    // '        End If
    // '        If recoverresult < 0 Then
    // '            recoverresult = TileDataRecovery(longMinimum(BMPFileHeader.bfSize + 1, startseek), f, longMaximum(Abs((BMPFileHeader.bfSize + 1) - startseek), 30))
    // '        End If
    // '
    // '        Seek #f, Abs(recoverresult)
    //
    //     End If
    //
    //     Call frmGeneral.UpdateProgress("Loading map", 100)
    //
    //     'search for walltiles, if exists and loaded , refresh
    //     'SearchWallTiles (FileName)
    //     'NOT NEEDED ANYMORE: Walltiles are in eLVL
    //
    //     Call walltiles.SetTileIsWalltile
    //     'End If
    //
    //     'update the tileset
    //     If usingDefaultTileset Then
    //         Call InitTileset("")
    //     Else
    //         Call frmGeneral.UpdateProgressLabel("Initializing tileset...")
    //
    //         Call InitTileset(filename)
    //     End If
    //
    //
    //



    auto pcurseek = in.device()->pos();
    auto pendseek = in.device()->size();

    qDebug() << QString("Loading %1 tiles at %2/%3...").arg(
        QString::number((pendseek - pcurseek)/4),
        QString::number(pcurseek),
        QString::number(pendseek));

    Q_ASSERT((pendseek - pcurseek) % 4 == 0); //Remaining bytes should be a multiple of 4 bytes

    int tileCount = 0;
    int badCount = 0;

    while (!in.atEnd() && in.status() == QDataStream::Ok)
    {
        SerializedTile t;

        in >> t.mBytes;

        if (t._unused1 != 0 || t._unused2 != 0)
        {
            //qWarning() << "Invalid tile data:" << intAsHexString(t.mBytes);
            badCount++;
            /// @todo Error message
        }
        else
        {
            if (in.status() == QDataStream::Status::Ok)
            {
                tileCount++;

                loadedTiles(t.mX, t.mY).mId = t.mTileId;
            }
            else
            {
                qWarning() << "Read error at" << in.device()->pos();
            }
        }
    };

    qDebug() << "Loaded" << tileCount << "tiles";

    if (badCount)
        qWarning() << "Loaded" << badCount << "bad tiles";

    if (in.status() != QDataStream::Ok)
        qWarning() << "Load error:" << in.status();

    //     AddDebug "OpenMap, tile data starting at " & Seek(f)
    //
    //     Call frmGeneral.UpdateProgress("Loading map", 200)
    //
    //     Call frmGeneral.UpdateProgressLabel("Loading tiles...")
    //
    //     Dim nrtiles As Long
    //     nrtiles = 0
    //
    //
    //     Dim loadcorrupttiles As Boolean, askedloadcorrupt As Boolean
    //     Dim corruptcount As Long
    //
    //
    //     'keep em coming till we have reached the end of the file
    //     Do Until EOF(f)
    //         Dim X As Integer, Y As Integer
    //
    //         'retrieve 4 bytes
    //         Get #f, , b
    //
    //         'extract the data
    //         X = (b(0) + 256 * (b(1) Mod 16)) 'Mod 1024
    //         Y = (b(1) \ 16 + 16 * b(2)) 'Mod 1024
    //
    //
    //         If X < 0 Or X > 1023 Or Y < 0 Or Y > 1023 Then
    //             'We have a corrupt tile
    //             If Not askedloadcorrupt Then
    //                 If MessageBox("Some tiles appear to be corrupted, do you want to load them anyway?", vbYesNo + vbQuestion) = vbYes Then
    //                     loadcorrupttiles = True
    //                 End If
    //                 askedloadcorrupt = True
    //             End If
    //
    //             If loadcorrupttiles Then
    //                 X = X Mod 1024
    //                 Y = Y Mod 1024
    //             End If
    //
    //             corruptcount = corruptcount + 1
    //         End If
    //
    //         If X >= 0 And X <= 1023 And Y >= 0 And Y <= 1023 Then
    //
    //             If tile(X, Y) > 0 Then
    //                 'tile has already come, skip it, prolly last tile
    //             ElseIf b(3) <> 0 Then
    //                 'last entry might be 0, just don't count it
    //
    //                 tile(X, Y) = b(3)
    //                 nrtiles = nrtiles + 1
    //
    //                 Call CompleteObject(Me, X, Y, undoch, False)
    //
    //             End If
    //         End If
    //     Loop
    //
    //     Call frmGeneral.UpdateProgress("Loading map...", 600)
    //
    //     AddDebug "OpenMap, " & nrtiles & " tiles loaded. Now at " & Seek(f)
    //
    //
    //     Close #f
    //
    //     If corruptcount > 0 Then
    //         AddDebug corruptcount & " corrupted tiles " & IIf(loadcorrupttiles, "loaded", "ignored")
    //
    //         MessageBox "A total of " & corruptcount & " corrupted tiles were " & IIf(loadcorrupttiles, "loaded.", "ignored."), vbInformation + vbOKOnly
    //     End If
    //
    //     'we load so we have a path
    //     activeFile = filename
    //
    //     AddDebug "OpenMap, File closed. Searching for matching cfg file"
    //     Call CFG.SearchCfg
    //
    //     'add this to recent
    //     AddDebug "OpenMap, Adding to recent list"
    //     Call frmGeneral.AddRecent(filename)
    //
    //     'just opened so the map hasn't changed yet
    //     mapchanged = False
    //
    //
    //     dontUpdateOnValueChange = True
    //     Call UpdateScrollbars(False)
    //
    //
    //     dontUpdateOnValueChange = False
    //
    //     AddDebug "OpenMap, Drawing tiles"
    //
    //     Call frmGeneral.UpdateProgressLabel("Drawing tiles...")
    //
    //     'draw the full map used for radar or usingpixels magnifier.zoom level
    //     Dim i As Integer
    //     Dim j As Integer
    //     For j = 0 To 1023
    //         For i = 0 To 1023
    //             If tile(i, j) <> 0 Then
    //                 'when an object is special, flag the other
    //                 'tiles to -1 so that we know we are in an object
    //
    //
    //                 'draw the complete pixel map
    //                 'Call setPixel(pic1024.hdc, i, j, TilePixelColor(tile(i, j)))
    //                 Call cpic1024.setPixelLong(i, j, TilePixelColor(tile(i, j)))
    //             End If
    //         Next
    //
    //         If j Mod 4 = 0 Then Call frmGeneral.UpdateProgress("Loading map", 601 + j)
    //     Next
    //
    // '1140      AddDebug "OpenMap, Saving revert copy"
    // '1150      Call SaveRevert
    //
    //
    //
    //     Call Form_Resize
    //
    //
    //     Call Regions.RedrawAllRegions
    //
    //     'same as with new map, go to center of the map
    //     Call SetFocusAt(512, 512, picPreview.width \ 2, picPreview.height \ 2, False)
    //
    //     undoredo.ResetRedo
    //     Call frmGeneral.UpdateMenuMaps
    //
    //     mdlDebug.AddDebug "Map loaded successfully"
    //
    //     Call UpdateLevel(False, True)
    //
    //     frmGeneral.IsBusy("frmMain" & id & ".OpenMap") = False
    //
    //
    //     On Error GoTo 0
    //     Exit Sub
    //
    //
    // 'Corrupt_File:
    // '    If mapcorrupted Then
    // '        'Error appeared more than once, ignore
    // '        Resume Next
    // '    Else
    // '        AddDebug "OpenMap, Map corrupted! Seek position: " & Seek(f) & " last (X,Y): (" & X & "," & Y & ") b(3)= [" & b(0) & "," & b(1) & "," & b(2) & "," & b(3) & "]"
    // '        mapcorrupted = True
    // '        If MessageBox(filename & " may be corrupted. Do you wish to attempt recovering data?", vbYesNo + vbCritical) = vbYes Then
    // '            Dim recovery As Long
    // '            recovery = TileDataRecovery(BMPFileHeader.bfSize + 1, f)
    // '            If recovery < 0 Then
    // '                If MessageBox(filename & " could not be recovered completly, do you still want to load the file?", vbQuestion + vbYesNo) = vbYes Then
    // '                    Seek #f, Abs(recovery)
    // '                Else
    // '                    GoTo AbortRecovery
    // '                End If
    // '            Else
    // '                Seek #f, Abs(recovery)
    // '            End If
    // '
    // '            Resume Next
    // '        Else
    // '            GoTo AbortRecovery
    // '        End If
    // '    End If
    // '
    // '    frmGeneral.IsBusy("frmMain" & id & ".OpenMap") = False
    // '    Exit Sub
    // '
    // 'AbortRecovery:
    // '    Close #f
    // '    frmGeneral.IsBusy("frmMain" & id & ".OpenMap") = False
    // '    Exit Sub
    //
    // OpenMap_Error:
    //     frmGeneral.IsBusy("frmMain" & id & ".OpenMap") = False
    //     HandleError Err, "frmMain.OpenMap " & filename
    // End Sub

    if (in.status() == QDataStream::Status::Ok)
    {
        //Assign loaded data
        mIsDirty = false;

        mTiles = loadedTiles;
        mExtraLevelData = loadedExtraLevelData;
        mTileset = loadedTileset;

        /// @todo Clear undo/redo stack

        return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////

bool LevelData::saveToFile(const QString& filepath) const
{

    mIsDirty = false;

    //     Sub SaveMap(path As String, Optional flags As saveFlags = SFdefault)
    // 'Saves the map
    // 'back the old one up
    // 'if there is already a backup, kill it
    // 'then rename the new old one to the .bak one
    //     On Error GoTo SaveMap_Error
    //
    //     AddDebug "SaveMap, saving map... " & path
    //     AddDebug "         Flags = " & flags
    //
    //     If FlagIs(flags, SFsilent) Then
    //         frmGeneral.SetMousePointer vbArrowHourglass
    //     Else
    //
    //         frmGeneral.IsBusy("frmMain" & id & ".SaveMap") = True
    //
    //         If FileExists(path) Then
    //             AddDebug "SaveMap, " & path & " exists, checking for .bak file"
    //             If DeleteFile(path & ".bak") Then
    //                   RenameFile path, path & ".bak"
    //             End If
    // '100               If FileExists(path & ".bak") Then
    // '110                   AddDebug "SaveMap, " & path & ".bak exists, killing it"
    // '120                   Kill path & ".bak"
    // '130               End If
    // '140               Name path As path & ".bak"
    //
    //             'Make sure the file is deleted
    //             DeleteFile path
    //         End If
    //
    //     End If
    //
    //
    //
    //     Dim f As Integer
    //     f = FreeFile
    //
    //     AddDebug "SaveMap, opening " & path & " for binary as #" & f
    //
    //
    //
    //     Open path For Binary As #f
    //
    //     On Error GoTo SaveMap_Error_Opened
    //
    //     '''''TILESET IS NOW ALWAYS SAVED IF WE HAVE ELVL DATA
    //
    //
    //
    //
    //     If (usingDefaultTileset And Not (eLVL.HasData Or lvz.getLVZCount > 0)) Or Not FlagIs(flags, SFsaveTileset) Then
    //         AddDebug "SaveMap, usingDefaultTileset ; no tileset saved"
    //         'no need to write the bmp
    //         'just begin writing the tile data
    //     Else
    //         If usingDefaultTileset Then
    //             'We'll need to import the default tileset as a new tileset
    //             frmGeneral.picdefaulttileset.Picture = frmGeneral.picdefaulttileset.Image
    //
    //             Dim temppath As String
    //             temppath = App.path & "\tempTileset" & GetTickCount & ".bmp"
    //             Call SavePicture(frmGeneral.picdefaulttileset.Picture, temppath)
    //
    //             Call ImportTileset(temppath)
    //
    //             DeleteFile temppath
    //         End If
    //
    //         Call frmGeneral.UpdateProgressLabel("Saving tileset...")
    //
    //         'TODO: IF SAVEASSSMECOMPATIBLE, CONVERT TO 8 BIT
    //         '##################
    //         '##################
    //         '##################
    //
    //         'put the bitmap data first
    //
    //         'bitmap header position is needed to replace it later when we know the size of eLVL data
    //         Dim headerPos As Long
    //         headerPos = Seek(f)
    //         AddDebug "SaveMap, bitmap header position: " & headerPos
    //
    //         Put #f, , bmpFileheader
    //         Put #f, , BMPInfoHeader
    //
    //         If BMPInfoHeader.biBitCount = 8 Then
    //             Put #f, , BMPRGBQuad
    //         End If
    //
    //         Put #f, , bmpdata
    //     End If
    //
    //     Call frmGeneral.UpdateProgress("Saving", 50)
    //
    //
    //
    //     If eLVL.HasData And FlagIsPartial(flags, SFsaveELVL) Then
    //
    //         'NOT TRUE ANYMORE >>>>>>>>>>>>>
    //         'Can't save eLVL with 24bit tileset
    //         'If BMPInfoHeader.biBitCount = 24 Then
    //         '    messagebox "eLVL data could not be saved because the tileset has a color depth of 24 bits. If you wish to save regions and attributes, you will need to convert your tileset to 8 bits using any drawing program, re-import it, and save the map again. You map will be saved without eLVL data for now.", vbOKOnly + vbExclamation, "eLVL data not saved"
    //         '<<<<<<<<<<<<<<<<<<<<<<
    //
    //         'Can't save eLVL without tileset
    //
    //         If usingDefaultTileset Or Not FlagIs(flags, SFsaveTileset) Then
    //             AddDebug "SaveMap, No tileset found in map, cannot save eLVL data"
    //             If Not FlagIs(flags, SFsilent) Then
    //                 MessageBox "eLVL data could not be saved because this map does not have a tileset. If you wish to save regions and attributes, you will need to to import a tileset, and save the map again. You map will be saved without eLVL data for now.", vbOKOnly + vbExclamation, "eLVL data not saved"
    //             End If
    //
    //         Else
    //             Call frmGeneral.UpdateProgressLabel("Saving eLVL data...")
    //
    //             ''''put eLVL data
    // '            If True Then  'UnsignedToInteger(eLVL.Next4bytes(Seek(f)) - 1) <= 32767
    //             bmpFileheader.bfReserved1 = UnsignedToLong(Next4bytes(Seek(f)) - 1)    'should be 49720 with 8bit tilesets, unless compression
    // '            Else
    // '                AddDebug "SaveMap, bfReserved1 could not be set to " & (eLVL.Next4bytes(Seek(f)) - 1)
    // '                messagebox "map not saved, view debug for more info"    'TEMPORARY ERROR MESSAGE
    // '                Close #f
    // '                Exit Sub
    // '            End If
    //
    //             Seek #f, Next4bytes(Seek(f))
    //
    //             AddDebug "SaveMap, bfReserved1 set to: " & bmpFileheader.bfReserved1 & " (" & LongToUnsigned(bmpFileheader.bfReserved1) & ")"
    //
    //             'Seek #f, IntegerToUnsigned(BMPFileHeader.bfReserved1) + 1
    //             Dim elvlsize As Long, orgbfSize As Long
    //
    //             orgbfSize = bmpFileheader.bfSize
    //
    //
    //             elvlsize = eLVL.PutELVLData(f, flags)
    //
    //             AddDebug "SaveMap, total eLVL size returned: " & elvlsize
    //
    //             'update bitmap filesize including elvl data
    //             bmpFileheader.bfSize = UnsignedToLong((LongToUnsigned(bmpFileheader.bfReserved1) + elvlsize))
    //
    //             AddDebug "SaveMap, New bitmap bfSize: " & bmpFileheader.bfSize & " (" & LongToUnsigned(bmpFileheader.bfSize) & ")"
    //
    //             ''''replace bitmap header with new values
    //             Seek #f, headerPos
    //             Put #f, , bmpFileheader
    //
    //             'once all the bmp data and elvl is put, continue at the position for
    //             'the map data
    //             Seek #f, LongToUnsigned(bmpFileheader.bfSize) + 1
    //
    //             'Reset header properties
    //             bmpFileheader.bfReserved1 = 0
    //             bmpFileheader.bfSize = orgbfSize
    //         End If
    //
    //     End If
    //
    //
    //
    //     Call frmGeneral.UpdateProgress("Saving", 100)
    //     Call frmGeneral.UpdateProgressLabel("Saving tiles...")
    //
    //     AddDebug "SaveMap, BMPData is stored into lvl, starting tile data at " & Seek(f)
    //
    //
    //     Dim nrtiles As Long
    //     nrtiles = 0
    //
    //     Dim X As Integer, Y As Integer, curtile As Integer
    //     Dim Data As Long, btile As Byte
    //
    //     Dim dontSaveExtraTiles As Boolean
    //
    // '    Dim tick As Long
    // '    tick = GetTickCount
    //
    //     dontSaveExtraTiles = Not FlagIs(flags, SFsaveExtraTiles)
    //
    //     For Y = 0 To 1023
    //
    //
    //         For X = 0 To 1023
    //
    //             curtile = tile(X, Y)
    //
    //             'watch out for special tile flagging !, but
    //             'those will be 0 anyway, so skip them too
    //             If curtile > 0 Then
    //
    //
    //                 'if ssme compatible is required, discard all tiles
    //                 'above 190 except for the ones that ssme does recognize
    //                 If dontSaveExtraTiles Then
    //                     If curtile > 190 And _
    //                         curtile <> 216 And curtile <> 217 And _
    //                         curtile <> 218 And curtile <> 219 And _
    //                         curtile <> 220 Then
    //
    //                         'do not save those not compatible with ssme
    //
    //
    //                     Else
    //                         'now put the bytes
    //
    //                         Data = (X Mod 256) + (Y \ 16) * &H10000 + (X \ 256) * &H100 + &H1000 * CLng(Y Mod 16)
    //
    //                         btile = curtile
    //
    //                         CopyMemory ByVal VarPtr(Data) + 3, ByVal VarPtr(btile), 1
    //
    //                         Put #f, , Data
    //
    // '                        Put #f, , CByte(X Mod 256)
    // '                        Put #f, , CByte((X \ 256) + (Y Mod 16) * 16)
    // '                        Put #f, , CByte(Y \ 16)
    // '
    // '                        Put #f, , CByte(curtile)
    //
    //
    //                         nrtiles = nrtiles + 1
    //                     End If
    //                 Else
    //
    // '
    //
    //
    //                     'Doing it this way only uses 1 'Put' instruction instead of 4, which reduces
    //                     'by almost 4x the execution time
    //                     'We have to use a CopyMemory for the MSB of the Long because otherwise we'd get
    //                     'overflow errors
    //                     Data = (X Mod 256) + (Y \ 16) * &H10000 + (X \ 256) * &H100 + &H1000 * CLng(Y Mod 16)
    //
    //                     btile = curtile
    //
    //                     CopyMemory ByVal VarPtr(Data) + 3, ByVal VarPtr(btile), 1
    //
    //                     Put #f, , Data
    //
    //                     'now put the bytes
    // '                    Put #f, , CByte(X Mod 256)
    // '                    Put #f, , CByte((X \ 256) + (Y Mod 16) * 16)
    // '                    Put #f, , CByte(Y \ 16)
    // '                    Put #f, , CByte(curtile)
    //
    //
    //                     nrtiles = nrtiles + 1
    //                 End If
    //             'Else
    //                 'dont save those empty tiles
    //             End If
    //         Next
    //
    // '        If Y Mod 4 = 0 Then Call frmGeneral.UpdateProgress("Saving", 101 + Y)
    //
    //     Next
    //
    //
    //     AddDebug "SaveMap, " & nrtiles & " tiles were saved into lvl. Total file size: " & Seek(f) & " bytes."
    //
    //     AddDebug "SaveMap, Closing file #" & f
    //     Close #f
    //
    //
    //     'save lvzs
    //     If FlagIs(flags, SFsaveLVZ) And lvz.getLVZCount > 0 Then
    //         Call frmGeneral.UpdateProgressLabel("Saving LVZ files...")
    //
    //         Dim i As Integer
    //         For i = 0 To lvz.getLVZCount - 1
    //
    //             Call lvz.exportLVZ(GetPathTo(path) & lvz.getLVZname(i), i)
    //
    //             Call frmGeneral.UpdateProgress("Saving", 1124 + (200 / lvz.getLVZCount) * (i + 1))
    //         Next
    //     End If
    //
    //     Call frmGeneral.UpdateProgress("Saving", 1324)
    //
    //     'reset autosave countdown
    //     MinutesCounted = 0
    //
    //
    //
    //     If FlagIs(flags, SFsaveExtraTiles) And Not FlagIs(flags, SFsilent) Then
    //         AddDebug "mapchanged set to False"
    //         mapchanged = False
    //         'else, no mapchanged, as some data could be lost
    //     End If
    //
    //     Call frmGeneral.UpdateMenuMaps
    //
    // '930       If FlagIs(flags, SFsaveRevert) Then
    // ''940           Call frmGeneral.UpdateProgressLabel("Creating backup for revert...")
    // ''950           Call SaveRevert
    // '960       End If
    //
    //     Call frmGeneral.UpdateProgress("Saving", 1524)
    //
    //     frmGeneral.IsBusy("frmMain" & id & ".SaveMap") = False
    //
    //
    //
    //     On Error GoTo 0
    //     Exit Sub
    //
    // SaveMap_Error_Opened:
    //     Close #f
    // SaveMap_Error:
    //
    //     frmGeneral.IsBusy("frmMain" & id & ".SaveMap") = False
    //
    //     If Err.Number = 75 Then
    //         MessageBox "Path/File access error 75" & vbCrLf & "You do not have permissions to write to '" & path & "'. Make sure you have the permission to write to this folder.", vbExclamation + vbOKOnly
    //         Exit Sub
    //     Else
    //         HandleError Err, "frmMain" & id & ".SaveMap " & path & " " & flags & "(" & X & "," & Y & ")", Not FlagIs(flags, SFsilent)
    //     End If
    // End Sub
    return false;
}

//////////////////////////////////////////////////////////////////////////

LevelCoords LevelData::boundPixelToLevel(const LevelCoords& pixel) const
{
    return bounds().bounded(pixel);
}

//////////////////////////////////////////////////////////////////////////

void LevelData::boundTileToLevel(int& tileX, int& tileY) const
{
    tileX = std::clamp(tileX, 0, size().width() - 1);
    tileY = std::clamp(tileY, 0, size().height() - 1);
}

//////////////////////////////////////////////////////////////////////////

TileCoords LevelData::boundTileToLevel(const TileCoords& tileXY) const
{
    return TileCoords(
        std::clamp(tileXY.x(), 0, size().width() - 1),
        std::clamp(tileXY.y(), 0, size().height() - 1)
    );
}

//////////////////////////////////////////////////////////////////////////

const Tileset& LevelData::tileset() const
{
    return mTileset;
}

//////////////////////////////////////////////////////////////////////////

void LevelData::setDirty(bool b)
{
    mIsDirty = b;
}

//////////////////////////////////////////////////////////////////////////

bool LevelData::isDirty() const
{
    return mIsDirty;
}
