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
#include "Log.h"
#include "Node.h"
#include "StaticSprite2D.h"
#include "TileLayer2D.h"
#include "TileMap2D.h"
#include <Tmx.h>

#include "DebugNew.h"

namespace Urho3D
{

TileLayer2D::TileLayer2D(Context* context) :
    Component(context)
{
}

TileLayer2D::~TileLayer2D()
{
}

void TileLayer2D::RegisterObject(Context* context)
{
    context->RegisterFactory<TileLayer2D>();
}

void TileLayer2D::SetTmxLayer(TileMap2D* tileMap, const Tmx::Layer* tmxLayer)
{
    if (tmxLayer == tmxLayer_)
        return;

    tileMap_ = tileMap;
    tmxLayer_ = tmxLayer;

    Node* node = GetNode();
    node->RemoveAllChildren();

    if (!tmxLayer_)
        return;

    int width = tmxLayer_->GetWidth();
    int height = tmxLayer_->GetHeight();

    float tileWidth = tileMap->GetTileWidth();
    float tileHeight = tileMap->GetTileHeight();

    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            const Tmx::MapTile& tile = tmxLayer_->GetTile(x, y);
            if (tile.id == 0)
                continue;

            Sprite2D* sprite = tileMap->GetTileSprite(tile.id + 1);
            if (!sprite)
            {
                LOGERROR("Could found sprite");
                return;
            }

            Node* tileNode = node->CreateChild("TileNode");
            // Set tile node position, need flip Y
            tileNode->SetPosition(Vector3(x * tileWidth, (height - 1 - y) * tileHeight, 0.0f));

            StaticSprite2D* tileStaticSprite = tileNode->CreateComponent<StaticSprite2D>();
            tileStaticSprite->SetSprite(sprite);
            tileStaticSprite->SetLayer(tmxLayer_->GetZOrder());
            tileStaticSprite->SetOrderInLayer(height - 1 - y);
        }
    }
}

}