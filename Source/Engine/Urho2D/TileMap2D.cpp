//
// Copyright (c) 2008-2014 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "Precompiled.h"
#include "Context.h"
#include "TileMap2D.h"

#include <Tmx.h>

#include "DebugNew.h"
#include "File.h"
#include "Log.h"
#include "TileLayer2D.h"
#include "Node.h"
#include "ResourceCache.h"
#include "OpenGL\OGLTexture2D.h"
#include "FileSystem.h"
#include "Sprite2D.h"
#include "Drawable2D.h"

namespace Urho3D
{

extern const char* URHO2D_CATEGORY;

TileMap2D::TileMap2D(Context* context) :
    Component(context),
    tmxMap_(new Tmx::Map())
{
}

TileMap2D::~TileMap2D()
{
    delete tmxMap_;
}

void TileMap2D::RegisterObject(Context* context)
{
    context->RegisterFactory<TileMap2D>(URHO2D_CATEGORY);
}

bool TileMap2D::SetTMXFile(File* tmxFile)
{
    tileSpriteMapping_.Clear();
    GetNode()->RemoveAllChildren();

    unsigned dataSize = tmxFile->GetSize();
    SharedArrayPtr<char> data(new char[dataSize]);
    if (tmxFile->Read(&data[0], dataSize) != dataSize)
        return false;
    
    tmxMap_->ParseText(&data[0]);
    if (tmxMap_->HasError())
    {
        LOGERROR("Load TMX file failed");
        return false;
    }

    for (int i = 0; i < tmxMap_->GetNumTilesets(); ++i)
    {
        const Tmx::Tileset* tmxTileset = tmxMap_->GetTileset(i);
        const Tmx::Image* tmxImage = tmxTileset->GetImage();
        
        String textureFileName = tmxImage->GetSource().c_str();
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        Texture2D* texture = cache->GetResource<Texture2D>(textureFileName, false);
        // If texture not found, try get in current directory
        if (!texture)
            texture = cache->GetResource<Texture2D>(GetParentPath(tmxFile->GetName()) + textureFileName);

        if (!texture)
            return false;

        int spacing = tmxTileset->GetSpacing();
        int tileWidth = tmxTileset->GetTileWidth();
        int tileHeight = tmxTileset->GetTileHeight();
        
        int x = spacing;
        int y = spacing;
        int id = tmxTileset->GetFirstGid();
        for (;;)
        {
            SharedPtr<Sprite2D> sprite(new Sprite2D(context_));
            sprite->SetTexture(texture);
            
            sprite->SetRectangle(IntRect(x, y, x + tileWidth, y + tileHeight));
            tileSpriteMapping_[id] = sprite;
            id += 1;

            x += spacing + tileWidth;
            if (x >= tmxImage->GetWidth())
            {
                x = spacing;
                y += spacing + tileHeight;
                if (y >= tmxImage->GetHeight())
                    break;
            }
        }
    }

    for (int i = 0; i < tmxMap_->GetNumLayers(); ++i)
    {
        Node* layerNode = GetNode()->CreateChild("TMXLayerNode");
        TileLayer2D* tileLayer = layerNode->CreateComponent<TileLayer2D>();
        tileLayer->SetTmxLayer(this, tmxMap_->GetLayer(i));
    }

    return true;
}

float TileMap2D::GetTileWidth() const
{
    return tmxMap_->GetTileWidth() * PIXEL_SIZE;
}

float TileMap2D::GetTileHeight() const
{
    return tmxMap_->GetTileHeight() * PIXEL_SIZE;
}

Sprite2D* TileMap2D::GetTileSprite(int gid) const
{
    HashMap<int, SharedPtr<Sprite2D> >::ConstIterator i = tileSpriteMapping_.Find(gid);
    if (i != tileSpriteMapping_.End())
        return i->second_;
    return 0;
}

Tmx::Map* TileMap2D::GetTMXMap() const
{
    return tmxMap_;
}

}
